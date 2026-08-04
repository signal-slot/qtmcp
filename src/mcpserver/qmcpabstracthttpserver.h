#ifndef QMCPABSTRACTHTTPSERVER_H
#define QMCPABSTRACTHTTPSERVER_H

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtMcpServer/qmcpserverglobal.h>
#include <QtNetwork/QNetworkRequest>

QT_BEGIN_NAMESPACE

class QTcpServer;

/*!
    \class QMcpAbstractHttpServer
    \inmodule QtMcpServer
    \brief The QMcpAbstractHttpServer class provides a base class for HTTP-based MCP servers.

    This class implements basic HTTP server functionality with support for Server-Sent Events (SSE),
    allowing real-time communication from server to client. It handles the low-level details of
    HTTP connections and SSE event streaming.

    To implement a custom HTTP server:
    \list
    \li Inherit from QMcpAbstractHttpServer
    \li Call bind() with a QTcpServer instance to start accepting connections
    \li Use the protected SSE methods to manage event streaming
    \endlist
*/
class Q_MCPSERVER_EXPORT QMcpAbstractHttpServer : public QObject
{
    Q_OBJECT

public:
    /*!
        Constructs an HTTP server with the given parent.
        \param parent The parent object
    */
    explicit QMcpAbstractHttpServer(QObject *parent = nullptr);

    /*!
        Destroys the HTTP server.
    */
    ~QMcpAbstractHttpServer() override;

    /*!
        Binds this server to the given TCP server.
        The TCP server must be listening before calling this method.

        \param server The TCP server to bind to
        \return true if binding was successful, false otherwise
    */
    bool bind(QTcpServer *server);

signals:
    /*!
        Emitted when a deferred or SSE connection is closed by the peer.
        On the Streamable HTTP transport (2026-07-28) closing a request's
        response stream is the cancellation signal for that request.

        \param id UUID of the closed connection
    */
    void connectionClosed(const QUuid &id);

protected:
    /*!
        Registers a new SSE request and returns a unique identifier for it.

        \param request The HTTP request to register
        \return UUID for the registered SSE connection
    */
    QUuid registerSseRequest(const QNetworkRequest &request);

    /*!
        Takes over the connection of \a request so the response can be sent
        later with completeResponse() or upgraded to an SSE stream with
        upgradeToSse(). Call from within a request slot; the automatic
        response is suppressed.

        \param request The HTTP request whose response is deferred
        \return UUID identifying the deferred connection, null on failure
    */
    QUuid deferResponse(const QNetworkRequest &request);

    /*!
        Completes a deferred response and releases the connection back to
        normal request parsing (HTTP keep-alive).

        \param id UUID returned by deferResponse()
        \param statusCode HTTP status code
        \param body Response body, may be empty
        \param contentType Content-Type when \a body is not empty
        \param extraHeaders Additional response headers, e.g. Mcp-Session-Id
    */
    void completeResponse(const QUuid &id, int statusCode, const QByteArray &body = {},
                          const QString &contentType = QStringLiteral("application/json"),
                          const QList<std::pair<QByteArray, QByteArray>> &extraHeaders = {});

    /*!
        Upgrades a deferred response to an SSE stream. Use sendSseEvent() to
        emit events and closeSseConnection() to end the stream.

        \param id UUID returned by deferResponse()
        \param extraHeaders Additional response headers
        \return true when the connection was upgraded
    */
    bool upgradeToSse(const QUuid &id, const QList<std::pair<QByteArray, QByteArray>> &extraHeaders = {});

    /*!
        Sends an SSE event to a specific client.
        
        \param id UUID of the SSE connection
        \param data The event data to send
        \param event Optional event type name
    */
    void sendSseEvent(const QUuid &id, const QByteArray &data, const QString &event = QString());

    /*!
        Sends an SSE comment line to a specific client. Comments are ignored by
        SSE clients and are the conventional way to keep an idle stream alive.

        \param id UUID of the SSE connection
        \param comment Comment text, may be empty
    */
    void sendSseComment(const QUuid &id, const QByteArray &comment = {});

    /*!
        Closes an SSE connection.
        
        \param id UUID of the SSE connection to close
    */
    void closeSseConnection(const QUuid &id);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPABSTRACTHTTPSERVER_H
