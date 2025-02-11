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
        qint64 receivedLength = 0;  // Track received data size
    };

    QMap<QTcpSocket*, ParseData> dataMap;
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

    socket->deleteLater();
}

void QMcpAbstractHttpServer::Private::parseHttpRequest(QTcpSocket *socket)
{
    const auto mo = q->metaObject();

    ParseData &data = dataMap[socket];
    QByteArray newData = socket->readAll();
    data.data.append(newData);
    data.receivedLength += newData.size();

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

    // Check if we have received all expected data
    if (data.contentLength >= 0 && data.receivedLength < data.contentLength) {
        return;  // Wait for more data
    }

    if (data.indexOfMethod < 0) {
        sendHttpResponse(socket, QByteArrayLiteral("Not Found"), QStringLiteral("text/plain"), 404);
        return;
    }

    auto mm = mo->method(data.indexOfMethod);
    QByteArray ret;
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
    sendHttpResponse(socket, ret, "text/plain"_L1, 200);
}

void QMcpAbstractHttpServer::Private::sendHttpResponse(QTcpSocket *socket, const QByteArray &data,
                                                     const QString &contentType, int statusCode)
{
    QString statusText = (statusCode == 200) ? QStringLiteral("OK") : QStringLiteral("Not Found");
    QByteArray response = QString(QStringLiteral("HTTP/1.1 %1 %2\r\n"
                                               "Content-Type: %3\r\n"
                                               "Content-Length: %4\r\n"
                                               "\r\n"))
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
        for (QTcpSocket *socket : d->dataMap.keys()) {
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
