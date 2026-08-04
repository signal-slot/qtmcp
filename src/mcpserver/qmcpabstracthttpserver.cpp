#include "qmcpabstracthttpserver.h"
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHttpHeaders>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QMap>

class QMcpAbstractHttpServer::Private
{
public:
    Private(QMcpAbstractHttpServer *parent);
    void handleNewConnection();
    void handleDisconnected(QTcpSocket *socket);
    void parseHttpRequest(QTcpSocket *socket);
    void sendHttpResponse(QTcpSocket *socket, const QByteArray &data,
                         const QString &contentType = QStringLiteral("text/plain"),
                         int statusCode = 200);

private:
    QMcpAbstractHttpServer *q;
public:
    QTcpServer *server = nullptr;
    struct ParseData {
        QByteArray data;
        QNetworkRequest request;
        int indexOfMethod = -1;
        qint64 contentLength = -1;  // Store expected content length
    };

    QMap<QTcpSocket*, ParseData> dataMap;
    QMap<QUuid, QTcpSocket*> sessions;
    QMap<QUuid, QTcpSocket*> deferred;

    // The socket whose request slot is currently running. The connection
    // cannot be identified by its QNetworkRequest alone because two clients
    // may issue byte-identical requests concurrently.
    QTcpSocket *currentSocket = nullptr;
    // Set by deferResponse() while currentSocket's slot runs, so that the
    // automatic response stays suppressed even when the slot already answered
    // synchronously through completeResponse().
    bool responseTakenOver = false;
};

QMcpAbstractHttpServer::Private::Private(QMcpAbstractHttpServer *parent)
    : q(parent)
{}

void QMcpAbstractHttpServer::Private::handleNewConnection()
{
    while (QTcpSocket *socket = server->nextPendingConnection()) {
        dataMap.insert(socket, ParseData());
        connect(socket, &QTcpSocket::readyRead, q, [this, socket]() {
            parseHttpRequest(socket);
        });
        connect(socket, &QTcpSocket::disconnected, q, [this, socket]() {
            handleDisconnected(socket);
        });

        if (socket->bytesAvailable() > 0)
            parseHttpRequest(socket);
    }
}

void QMcpAbstractHttpServer::Private::handleDisconnected(QTcpSocket *socket)
{
    if (!socket)
        return;

    if (dataMap.contains(socket)) {
        dataMap.remove(socket);
    }
    if (currentSocket == socket)
        currentSocket = nullptr;

    // Deferred and SSE connections are tracked by UUID; tell the subclass the
    // peer went away, e.g. to treat it as cancellation (2026-07-28).
    auto id = deferred.key(socket);
    if (!id.isNull()) {
        deferred.remove(id);
        emit q->connectionClosed(id);
    }
    id = sessions.key(socket);
    if (!id.isNull()) {
        sessions.remove(id);
        emit q->connectionClosed(id);
    }

    socket->deleteLater();
}

void QMcpAbstractHttpServer::Private::parseHttpRequest(QTcpSocket *socket)
{
    const auto mo = q->metaObject();

    ParseData &data = dataMap[socket];
    data.data.append(socket->readAll());

    if (!data.request.url().isValid()) {
        int cr = data.data.indexOf('\r');
        int lf = data.data.indexOf('\n');
        if (lf < 0) {
            return;
        }
        if (cr + 1 != lf) {
            qWarning() << cr << lf << data.data;
        }
        // Parse request line
        const QList<QByteArray> requestLine = data.data.left(cr).split(' ');
        if (requestLine.size() < 3) {
            qWarning() << requestLine;
            return;
        }

        QByteArray method = requestLine.at(0).trimmed();
        QByteArray path = requestLine.at(1).trimmed();

        // parse headers
        QHttpHeaders headers;
        int prevLf = 0;
        while (prevLf < data.data.length()) {
            prevLf = lf + 1;
            cr = data.data.indexOf('\r', prevLf);
            lf = data.data.indexOf('\n', prevLf);
            if (cr + 1 != lf) {
                qWarning() << cr << lf << prevLf << data.data;
            }
            if (cr == prevLf)
                break;

            QByteArray header = data.data.mid(prevLf, cr - prevLf);
            int colon = header.indexOf(':');
            if (colon < 0) {
                continue;
            }
            headers.append(QString::fromLatin1(header.left(colon)), QString::fromUtf8(header.mid(colon + 1).trimmed()));
        }

        // Extract Content-Length if present
        if (headers.contains("Content-Length"_L1)) {
            bool ok;
            const auto contentLengthStr = headers.value("Content-Length"_L1);
            data.contentLength = contentLengthStr.toLongLong(&ok);
            if (!ok) {
                data.contentLength = -1;  // No valid Content-Length header
            }
        }

        QUrl url;
        int question = path.indexOf('?');
        if (question < 0) {
            url.setPath(QString::fromUtf8(path));
        } else {
            url.setPath(QString::fromUtf8(path.left(question)));
            url.setQuery(QString::fromUtf8(path.mid(question + 1)));
        }

        data.request = QNetworkRequest(url);
        data.request.setHeaders(headers);
        data.data = data.data.remove(0, prevLf + 2);

        QByteArray slotName = method.toLower();
        const auto pathElements = url.path().split("/"_L1, Qt::SkipEmptyParts);
        for (const auto &pe : pathElements) {
            slotName += pe.toUpper().toUtf8().at(0);
            slotName += pe.toUtf8().mid(1).toLower();
        }

        for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
            if (mo->method(i).name() == slotName) {
                data.indexOfMethod = i;
                break;
            }
        }
    }

    // Check if we have received all expected data. data.data holds the body
    // only; the request line and headers were stripped above.
    if (data.contentLength >= 0 && data.data.size() < data.contentLength) {
        return;  // Wait for more data
    }

    if (data.indexOfMethod < 0) {
        sendHttpResponse(socket, "Not Found"_ba, QStringLiteral("text/plain"), 404);
        return;
    }

    auto mm = mo->method(data.indexOfMethod);
    QByteArray ret;
    currentSocket = socket;
    responseTakenOver = false;
    switch (mm.parameterCount()) {
    case 0:
        mm.invoke(q
                  , Qt::DirectConnection
                  , Q_RETURN_ARG(QByteArray, ret)
                  );
        break;
    case 1:
        mm.invoke(q
                  , Qt::DirectConnection
                  , Q_RETURN_ARG(QByteArray, ret)
                  , Q_ARG(QNetworkRequest, data.request)
                  );
        break;
    case 2:
        mm.invoke(q
                  , Qt::DirectConnection
                  , Q_RETURN_ARG(QByteArray, ret)
                  , Q_ARG(QNetworkRequest, data.request)
                  , Q_ARG(QByteArray, data.data)
                  );
        data.data.clear();
        break;
    default:
        qFatal();
    }
    currentSocket = nullptr;
    const bool takenOver = responseTakenOver;
    responseTakenOver = false;
    if (takenOver) {
        // The slot took over the connection via deferResponse(); the response
        // was either already sent from within the slot or is sent later
        // through completeResponse() / upgradeToSse().
    } else if (sessions.key(socket).isNull()) {
        sendHttpResponse(socket, ret, "text/plain"_L1, 200);
    } else {
        socket->write(ret);
    }
    dataMap.insert(socket, ParseData());
}

void QMcpAbstractHttpServer::Private::sendHttpResponse(QTcpSocket *socket, const QByteArray &data,
                                                     const QString &contentType, int statusCode)
{
    QString statusText = (statusCode == 200) ? "OK"_L1 : "Not Found"_L1;
    QByteArray response = u"HTTP/1.1 %1 %2\r\n"
                          "Content-Type: %3\r\n"
                          "Content-Length: %4\r\n"
                          "\r\n"_s
                              .arg(statusCode)
                              .arg(statusText)
                              .arg(contentType)
                              .arg(data.size())
                              .toLatin1();
    response += data;
    socket->write(response);
    socket->flush();
}

QMcpAbstractHttpServer::QMcpAbstractHttpServer(QObject *parent)
    : QObject{parent}
    , d(new Private(this))
{}

QMcpAbstractHttpServer::~QMcpAbstractHttpServer()
{}

bool QMcpAbstractHttpServer::bind(QTcpServer *server)
{
    static QMetaObject::Connection connection;

    if (d->server) {
        disconnect(connection);
        connection = QMetaObject::Connection();
        // Clean up any existing connections
        const auto sockets = d->dataMap.keys();
        for (QTcpSocket *socket : sockets) {
            socket->disconnect();
            socket->deleteLater();
        }
        d->dataMap.clear();
    }

    d->server = server;
    if (d->server) {
        while (server->hasPendingConnections())
            d->handleNewConnection();
        connection = connect(server, &QTcpServer::newConnection, this, [this]() {
            d->handleNewConnection();
        });
    }
    return true;
}

QUuid QMcpAbstractHttpServer::registerSseRequest(const QNetworkRequest &request)
{
    QUuid ret;
    static QByteArray response = QByteArrayLiteral("HTTP/1.1 200 OK\r\n"
                                            "Content-Type: text/event-stream\r\n"
                                            "Cache-Control: no-cache\r\n"
                                            "Connection: keep-alive\r\n"
                                            "\r\n");
    QTcpSocket *target = d->currentSocket;
    if (!target) {
        // Called outside of a request slot: fall back to matching the request.
        const auto sockets = d->dataMap.keys();
        for (QTcpSocket *socket : sockets) {
            if (d->dataMap.value(socket).request == request) {
                target = socket;
                break;
            }
        }
    }
    if (target) {
        ret = QUuid::createUuid();
        d->sessions.insert(ret, target);
        target->write(response);
        target->flush();
    } else {
        qWarning() << "sse socket for" << request.url() << "not found";
    }
    return ret;
}

void QMcpAbstractHttpServer::sendSseEvent(const QUuid &id, const QByteArray &data,
                                         const QString &event)
{
    if (!d->sessions.contains(id)) {
        qWarning() << "sse" << id << "not found";
        return;
    }
    auto *socket = d->sessions.value(id);
    QByteArray message;
    if (!event.isEmpty())
        message += "event: " + event.toUtf8() + "\r\n";
    message += "data: " + data + "\r\n\r\n";
    socket->write(message);
    socket->flush();
}

void QMcpAbstractHttpServer::sendSseComment(const QUuid &id, const QByteArray &comment)
{
    if (!d->sessions.contains(id)) {
        qWarning() << "sse" << id << "not found";
        return;
    }
    auto *socket = d->sessions.value(id);
    socket->write(": " + comment + "\r\n\r\n");
    socket->flush();
}

QUuid QMcpAbstractHttpServer::deferResponse(const QNetworkRequest &request)
{
    if (!d->currentSocket) {
        qWarning() << "deferResponse() for" << request.url()
                   << "must be called from within a request slot";
        return {};
    }
    const QUuid ret = QUuid::createUuid();
    d->deferred.insert(ret, d->currentSocket);
    d->responseTakenOver = true;
    return ret;
}

void QMcpAbstractHttpServer::completeResponse(const QUuid &id, int statusCode, const QByteArray &body,
                                              const QString &contentType,
                                              const QList<std::pair<QByteArray, QByteArray>> &extraHeaders)
{
    if (!d->deferred.contains(id)) {
        qWarning() << "deferred response" << id << "not found";
        return;
    }
    auto *socket = d->deferred.take(id);

    QByteArray statusText;
    switch (statusCode) {
    case 200: statusText = "OK"_ba; break;
    case 202: statusText = "Accepted"_ba; break;
    case 400: statusText = "Bad Request"_ba; break;
    case 403: statusText = "Forbidden"_ba; break;
    case 404: statusText = "Not Found"_ba; break;
    case 405: statusText = "Method Not Allowed"_ba; break;
    default: statusText = "Unknown"_ba; break;
    }

    QByteArray response = "HTTP/1.1 " + QByteArray::number(statusCode) + ' ' + statusText + "\r\n";
    if (!body.isEmpty())
        response += "Content-Type: " + contentType.toLatin1() + "\r\n";
    response += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
    for (const auto &header : extraHeaders)
        response += header.first + ": " + header.second + "\r\n";
    response += "\r\n";
    response += body;
    socket->write(response);
    socket->flush();
}

bool QMcpAbstractHttpServer::upgradeToSse(const QUuid &id, const QList<std::pair<QByteArray, QByteArray>> &extraHeaders)
{
    if (!d->deferred.contains(id)) {
        qWarning() << "deferred response" << id << "not found";
        return false;
    }
    auto *socket = d->deferred.take(id);
    d->sessions.insert(id, socket);

    QByteArray response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/event-stream\r\n"
                          "Cache-Control: no-cache\r\n"
                          "Connection: keep-alive\r\n"
                          "X-Accel-Buffering: no\r\n"_ba;
    for (const auto &header : extraHeaders)
        response += header.first + ": " + header.second + "\r\n";
    response += "\r\n";
    socket->write(response);
    socket->flush();
    return true;
}

void QMcpAbstractHttpServer::closeSseConnection(const QUuid &id)
{
    if (!d->sessions.contains(id)) {
        qWarning() << "sse" << id << "not found";
        return;
    }
    auto *socket = d->sessions.take(id);
    d->dataMap.remove(socket);
    socket->close();
    socket->deleteLater();
    return;
}

