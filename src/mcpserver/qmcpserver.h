// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVER_H
#define QMCPSERVER_H

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QFuture>
#include <QtCore/QObject>
#include <QtMcpCommon/QMcpJSONRPCErrorError>
#include <QtMcpCommon/QMcpJSONRPCResponse>
#include <QtMcpCommon/QMcpNotification>
#include <QtMcpCommon/QMcpRequest>
#include <QtMcpCommon/QMcpResource>
#include <QtMcpCommon/QMcpResult>
#include <QtMcpCommon/QMcpServerCapabilities>
#include <QtMcpCommon/QMcpTool>
#include <QtMcpServer/qmcpserverglobal.h>
#include <QtMcpServer/qmcpserversession.h>
#include <concepts>
#include <functional>
#include <type_traits>

QT_BEGIN_NAMESPACE

#ifdef QT_GUI_LIB
class QAction;
#endif

/*!
    \class QMcpServer
    \inmodule QtMcpServer
    \brief The QMcpServer class provides a server implementation for the Model Context Protocol (MCP).

    QMcpServer enables applications to act as MCP servers, handling client connections,
    requests, and notifications. It supports session management, tool registration,
    and resource handling.

    Example usage:
    \code
    // Create server with stdio backend
    auto server = new QMcpServer("stdio");

    // Configure server capabilities
    QMcpServerCapabilities capabilities;
    capabilities.setExperimental(true);
    server->setCapabilities(capabilities);
    server->setProtocolVersion("1.0");

    // Handle client requests
    server->addRequestHandler([](const QUuid &session, const QMcpInitializeRequest &req,
                               QMcpJSONRPCErrorError *error) {
        QMcpInitializeResult result;
        result.setProtocolVersion("1.0");
        return result;
    });

    // Start the server
    server->start("--log-level=debug");
    \endcode

    \sa QMcpClient
*/
class Q_MCPSERVER_EXPORT QMcpServer : public QObject
{
    Q_OBJECT
    /*!
        \property QMcpServer::capabilities
        This property holds the server's capabilities configuration.
        
        The capabilities define what features the server supports, such as
        experimental features, tools, resources, etc.
    */
    Q_PROPERTY(QMcpServerCapabilities capabilities READ capabilities WRITE setCapabilities NOTIFY capabilitiesChanged FINAL)

    /*!
        \property QMcpServer::instructions
        This property holds the server's instructions for clients.
        
        These instructions can provide guidance on how to interact with the server
        or describe its purpose.
    */
    Q_PROPERTY(QString instructions READ instructions WRITE setInstructions NOTIFY instructionsChanged FINAL)

    /*!
        \property QMcpServer::protocolVersion
        This property holds the MCP protocol version supported by the server.
        
        This version is used for compatibility checking with clients.
    */
    Q_PROPERTY(QString protocolVersion READ protocolVersion WRITE setProtocolVersion NOTIFY protocolVersionChanged FINAL)
public:
    /*!
        Returns a list of available backend implementations for the MCP server.
    */
    static QStringList backends();

    /*!
        Constructs an MCP server using the specified backend.
        
        \param backend Name of the backend implementation to use
        \param parent Parent QObject (optional)
    */
    explicit QMcpServer(const QString &backend, QObject *parent = nullptr);
    
    /*!
        Destroys the MCP server.
    */
    ~QMcpServer() override;

    /*!
        \internal
        Helper struct for extracting callback argument types.
    */
    template<typename> struct CallbackArg;

    template<typename T, typename Arg>
    struct CallbackArg<void(T::*)(const QUuid &, const Arg &) const> {
        using type = Arg;
    };

    template<typename T>
    struct CallbackArg : CallbackArg<decltype(&T::operator())> {};

    template<typename Callback>
    using CallbackResult = typename CallbackArg<Callback>::type;

    /*!
        Sends a request to a specific client session and handles the response with a callback.

        The callback will be invoked with the session ID and response result.

        Example:
        \code
        server->request(sessionId, QMcpInitializeRequest(), [](const QUuid &session, const QMcpInitializeResult &result) {
            qDebug() << "Session" << session << "initialized with version:" << result.protocolVersion();
        });
        \endcode

        \param session UUID of the client session
        \param request Request object inheriting from QMcpRequest
        \param callback Callback function to handle the response
    */
    template<typename Request, typename Callback>
        requires std::invocable<Callback, const QUuid &, const CallbackResult<Callback>&>
    void request(const QUuid &session, const Request &request, Callback callback)
    {
        using Result = CallbackResult<Callback>;

        static_assert(std::is_base_of<QMcpRequest, Request>::value, "Request must inherit from QMcpRequest");
        static_assert(std::is_base_of<QMcpResult, Result>::value, "Result must inherit from QMcpResult");

        auto json = request.toJsonObject();
        send(session, json, [callback](const QUuid & session, const QJsonObject &json) {
            Result result;
            result.fromJsonObject(json);
            callback(session, result);
        });
    }

    /*!
        Sends a request without expecting a response.

        This overload is useful for broadcast requests that don't target a specific session.

        \param request Request object inheriting from QMcpRequest
    */
    template<typename Request>
    void request(const QUuid &session, const Request &request)
    {
        static_assert(std::is_base_of<QMcpRequest, Request>::value, "Request must inherit from QMcpRequest");

        auto json = request.toJsonObject();
        send(session, json);
    }

    /*!
        Sends a notification to all connected clients.

        Notifications are one-way messages that do not expect a response.

        Example:
        \code
        QMcpLoggingMessageNotification notification;
        notification.setLevel(QMcpLoggingLevel::Info);
        notification.setMessage("Server broadcast message");
        server->notify(notification);
        \endcode

        \param notification Notification object inheriting from QMcpNotification
    */
    template<typename Notification>
    void notify(const QUuid &session, const Notification &notification)
    {
        static_assert(std::is_base_of<QMcpNotification, Notification>::value, "Notification must inherit from QMcpNotification");

        auto json = notification.toJsonObject();
        send(session, json);
    }


    template <typename T> struct RequestHandlerTraits;

    // Helper to detect QFuture
    template<typename T>
    struct is_future : std::false_type {};
    
    template<typename T>
    struct is_future<QFuture<T>> : std::true_type {};
    
    template <typename R, typename C, typename Arg>
    struct RequestHandlerTraits<R(C::*)(const QUuid &, Arg, QMcpJSONRPCErrorError *) const> {
        using RequestType = std::decay_t<Arg>;
        using ResultType = std::decay_t<R>;
    };

    template <typename Handler>
    void addRequestHandler(Handler handler)
    {
        using Traits = RequestHandlerTraits<decltype(&Handler::operator())>;
        using Req = typename Traits::RequestType;
        using Result = typename Traits::ResultType;

        static_assert(std::is_base_of<QMcpRequest, Req>::value,
                      "Request type must inherit from QMcpRequest");
        
        if constexpr (is_future<Result>::value) {
            static_assert(std::is_base_of<QMcpResult, typename Result::value_type>::value,
                          "Future result type must inherit from QMcpResult");
        } else {
            static_assert(std::is_base_of<QMcpResult, std::decay_t<Result>>::value,
                          "Result type must inherit from QMcpResult");
        }

        auto wrapper = [this, handler](const QUuid &session, const QJsonObject &json, QMcpJSONRPCErrorError *error) -> QJsonValue {
            Req req;
            req.fromJsonObject(json);
            
            if constexpr (is_future<Result>::value) {
                // For async handlers
                auto future = handler(session, req, error);
                
                // Get the request ID from the JSON object
                const auto id = json.value("id"_L1);
                
                // Set up continuation to send response when ready
                future.then([this, session, id](const auto &result) {
                    QMcpJSONRPCResponse response;
                    response.setId(id.toVariant());
                    auto object = response.toJsonObject();
                    object.insert("result"_L1, result.toJsonObject());
                    send(session, object);
                });
                
                // Return empty value since we'll send response later
                return QJsonValue();
            } else {
                // For sync handlers
                auto res = handler(session, req, error);
                return QJsonValue(res.toJsonObject());
            }
        };

        registerRequestHandler(Req().method(), wrapper);
    }

    template <typename T> struct NotificationHandlerTraits;

    template <typename C, typename Arg>
    struct NotificationHandlerTraits<void(C::*)(const QUuid &, Arg) const> {
        using NotificationType = std::decay_t<Arg>;
    };

    template <typename Handler>
    void addNotificationHandler(Handler handler)
    {
        using Traits = NotificationHandlerTraits<decltype(&Handler::operator())>;
        using Notification = typename Traits::NotificationType;

        static_assert(std::is_base_of<QMcpNotification, Notification>::value,
                      "Notification type must inherit from QMcpNotification");

        auto wrapper = [handler](const QUuid &session, const QJsonObject &json) {
            Notification notification;
            notification.fromJsonObject(json);
            handler(session, notification);
        };

        registerNotificationHandler(Notification().method(), wrapper);
    }

    /*!
        Returns the current server capabilities.
        \sa setCapabilities()
    */
    QMcpServerCapabilities capabilities() const;

    /*!
        Returns the current server instructions.
        \sa setInstructions()
    */
    QString instructions() const;

    /*!
        Returns the current protocol version.
        \sa setProtocolVersion()
    */
    QString protocolVersion() const;

    /*!
        Returns a mapping of feature identifiers to their toolDescriptions.
        Can be overridden by derived classes to provide custom toolDescriptions.
    */
    virtual QHash<QString, QString> toolDescriptions() const;

    QList<QMcpServerSession *> sessions() const;

public slots:
    /*!
        Sets the server capabilities.
        \param capabilities The new capabilities configuration
        \sa capabilities()
    */
    void setCapabilities(const QMcpServerCapabilities &capabilities);
    /*!
        Sets the server instructions.
        \param instructions The new instructions text
        \sa instructions()
    */
    void setInstructions(const QString &instructions);

    /*!
        Sets the protocol version.
        \param protocolVersion The new protocol version string
        \sa protocolVersion()
    */
    void setProtocolVersion(const QString &protocolVersion);

    /*!
        Starts the MCP server with the given arguments.
        \param args Command-line style arguments to pass to the backend (e.g., "--log-level=debug")
    */
    void start(const QString &args = QString());

    void registerToolSet(QObject *toolSet, const QHash<QString, QString> &descriptions = {});
    void unregisterToolSet(QObject *toolSet);
#ifdef QT_GUI_LIB
    void registerTool(QAction *action, const QString &name = QString());
    void unregisterTool(QAction *action);
#endif

signals:
    /*!
        Emitted when the server capabilities change.
        \param capabilities The new capabilities configuration
    */
    void capabilitiesChanged(const QMcpServerCapabilities &capabilities);
    /*!
        Emitted when the server instructions change.
        \param instructions The new instructions text
    */
    void instructionsChanged(const QString &instructions);

    /*!
        Emitted when the protocol version changes.
        \param protocolVersion The new protocol version string
    */
    void protocolVersionChanged(const QString &protocolVersion);

    /*!
        Emitted when the server has successfully started.
    */
    void started();

    /*!
        Emitted when a new client session is established.
        \param session The new session object
    */
    void newSession(QMcpServerSession *session);

    /*!
        Emitted when a raw JSON message is received from a client.
        \param session UUID of the client session
        \param object The received JSON message
    */
    void received(const QUuid &session, const QJsonObject &object);

    /*!
        Emitted when a result is ready to be sent to a client.
        \param session UUID of the client session
        \param result The result as a JSON object
    */
    void result(const QUuid &session, const QJsonObject &result);

private:
    void notifyResourceUpdated(const QUuid &session, const QMcpResource &resource);
    void send(const QUuid &session, const QJsonObject &message, std::function<void(const QUuid &session, const QJsonObject &)> callback = nullptr);
    void registerRequestHandler(const QString &method, std::function<QJsonValue(const QUuid &, const QJsonObject &, QMcpJSONRPCErrorError *)>);
    void registerNotificationHandler(const QString &method, std::function<void(const QUuid &, const QJsonObject &)>);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPSERVER_H
