// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENT_H
#define QMCPCLIENT_H

#include <QtMcpClient/qmcpclientglobal.h>
#include <QtCore/QObject>
#include <QtMcpCommon/QMcpRequest>
#include <QtMcpCommon/QMcpResult>
#include <QtMcpCommon/QMcpNotification>
#include <QtMcpCommon/QMcpJSONRPCErrorError>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <concepts>
#include <functional>

QT_BEGIN_NAMESPACE

/*!
    \class QMcpClient
    \inmodule QtMcpClient
    \brief The QMcpClient class provides a client implementation for the Model Context Protocol (MCP).

    QMcpClient enables applications to communicate with MCP servers using a type-safe,
    callback-based API. It supports sending requests and notifications, and handling
    responses through callbacks.

    Example usage:
    \code
    // Create client with stdio backend
    auto client = new QMcpClient("stdio");

    // Handle initialization
    client->request(QMcpInitializeRequest(), [](const QMcpInitializeResult &result,
                                              const QMcpJSONRPCErrorError *error) {
        if (error) {
            qDebug() << "Initialization failed:" << error->message();
            return;
        }
        qDebug() << "Initialized with protocol version:" << result.protocolVersion();
    });

    // Send notification
    QMcpLoggingMessageNotification notification;
    notification.setLevel(QMcpLoggingLevel::Info);
    notification.setMessage("Hello from client!");
    client->notify(notification);

    // Start the client
    client->start("--log-level=debug");
    \endcode

    \sa QMcpServer
*/
class Q_MCPCLIENT_EXPORT QMcpClient : public QObject
{
    Q_OBJECT

    /*!
        \property QMcpServer::protocolVersion
        This property holds the MCP protocol version supported by the server.

        This version is used for compatibility checking with clients.
    */
    Q_PROPERTY(QtMcp::ProtocolVersion protocolVersion READ protocolVersion WRITE setProtocolVersion NOTIFY protocolVersionChanged FINAL)

    /*!
        \property QMcpServer::supportedProtocolVersions
        This property holds the list of MCP protocol versions supported by the server.

        This list is used during the initialization handshake to negotiate
        a compatible protocol version with clients.
    */
    Q_PROPERTY(QList<QtMcp::ProtocolVersion> supportedProtocolVersions READ supportedProtocolVersions CONSTANT FINAL)

public:
    /*!
        Returns a list of available backend implementations for the MCP client.
    */
    static QStringList backends();

    /*!
        Constructs an MCP client using the specified backend.

        \param backend Name of the backend implementation to use
        \param parent Parent QObject (optional)
    */
    explicit QMcpClient(const QString &backend, QObject *parent = nullptr);

    /*!
        Destroys the MCP client.
    */
    ~QMcpClient() override;

    /*!
        Returns the current protocol version used by the client.
    */
    QtMcp::ProtocolVersion protocolVersion() const;

    /*!
        Returns a list of protocol versions supported by this client.
    */
    QList<QtMcp::ProtocolVersion> supportedProtocolVersions() const;

    /*!
        \internal
        Helper struct for extracting callback argument types.
    */
    template<typename> struct CallbackArg;

    template<typename T, typename Arg>
    struct CallbackArg<void(T::*)(const Arg &, const QMcpJSONRPCErrorError *) const> {
        using type = Arg;
    };

    template<typename T>
    struct CallbackArg : CallbackArg<decltype(&T::operator())> {};

    template<typename Callback>
    using CallbackResult = typename CallbackArg<Callback>::type;

    /*!
        Sends a request to the server and handles the response with a callback.

        The callback will be invoked with the response result and any error that occurred.
        If no error occurred, the error parameter will be nullptr.

        Example:
        \code
        client->request(QMcpInitializeRequest(), [](const QMcpInitializeResult &result,
                                                  const QMcpJSONRPCErrorError *error) {
            if (error) {
                qDebug() << "Error:" << error->message();
                return;
            }
            qDebug() << "Success:" << result.protocolVersion();
        });
        \endcode

        \param request Request object inheriting from QMcpRequest
        \param callback Callback function to handle the response
    */
    template<typename Request, typename Callback>
        requires std::invocable<Callback, const CallbackResult<Callback> &, const QMcpJSONRPCErrorError *>
    void request(const Request &request, Callback callback)
    {
        using Result = CallbackResult<Callback>;

        static_assert(std::is_base_of<QMcpRequest, Request>::value, "Request must inherit from QMcpRequest");
        static_assert(std::is_base_of<QMcpResult, Result>::value, "Result must inherit from QMcpResult");

        // For initialize requests, we'll handle protocol version negotiation in the send method
        // For all other requests, we use the current protocol version
        auto json = request.toJsonObject(protocolVersion());
        send(json, [callback, this](const QJsonObject &json, const QJsonObject &error) {
            // Use the negotiated protocol version from the response when available
            QtMcp::ProtocolVersion versionToUse = protocolVersion();

            // If the result contains a protocol version field, use that version
            if (json.contains("protocolVersion"_L1)) {
                QString resultVersion = json.value("protocolVersion"_L1).toString();
                QtMcp::ProtocolVersion resultVerEnum = QtMcp::stringToProtocolVersion(resultVersion);
                if (supportedProtocolVersions().contains(resultVerEnum)) {
                    versionToUse = resultVerEnum;
                }
            }

            Result result;
            result.fromJsonObject(json, versionToUse);
            if (!error.isEmpty()) {
                QMcpJSONRPCErrorError e;
                e.fromJsonObject(error, versionToUse);
                callback(result, &e);
            } else {
                callback(result, nullptr);
            }
        });
    }

    /*!
        Sends a request to the server without expecting a response.

        This overload is useful for fire-and-forget requests where no response handling is needed.

        \param request Request object inheriting from QMcpRequest
    */
    template<typename Request>
    void request(const Request &request)
    {
        static_assert(std::is_base_of<QMcpRequest, Request>::value, "Request must inherit from QMcpRequest");

        // Use the current protocol version for sending the request
        auto json = request.toJsonObject(protocolVersion());
        send(json);
    }

    /*!
        Sends a notification to the server.

        Notifications are one-way messages that do not expect a response.

        Example:
        \code
        QMcpLoggingMessageNotification notification;
        notification.setLevel(QMcpLoggingLevel::Info);
        notification.setMessage("Hello from client!");
        client->notify(notification);
        \endcode

        \param notification Notification object inheriting from QMcpNotification
    */
    template<typename Notification>
    void notify(const Notification &notification)
    {
        static_assert(std::is_base_of<QMcpNotification, Notification>::value, "Notification must inherit from QMcpNotification");

        // Use the current protocol version for sending the notification
        auto json = notification.toJsonObject(protocolVersion());
        send(json);
    }


    /*!
        \internal
        Helper struct for extracting request handler argument types.
    */
    template <typename T> struct RequestHandlerTraits;

    /*!
        \internal
        Specialization for member function pointers.
    */
    template <typename R, typename C, typename Arg>
    struct RequestHandlerTraits<R(C::*)(Arg, QMcpJSONRPCErrorError *) const> {
        using RequestType = std::decay_t<Arg>;
        using ResultType = std::decay_t<R>;
    };

    /*!
        Registers a handler for incoming requests.

        The handler will be called when a request with the corresponding method is received.
        The handler must return a Result object and can set an error if needed.

        Example:
        \code
        client->addRequestHandler([](const QMcpInitializeRequest &req, QMcpJSONRPCErrorError *error) {
            QMcpInitializeResult result;
            result.setProtocolVersion("1.0");
            return result;
        });
        \endcode

        \param handler Lambda or function object to handle requests
    */
    template <typename Handler>
    void addRequestHandler(Handler handler)
    {
        using Traits = RequestHandlerTraits<decltype(&Handler::operator())>;
        using Req = typename Traits::RequestType;
        using Res = typename Traits::ResultType;

        static_assert(std::is_base_of<QMcpRequest, Req>::value,
                      "Request type must inherit from QMcpRequest");
        static_assert(std::is_base_of<QMcpResult, Res>::value,
                      "Result type must inherit from QMcpResult");

        auto wrapper = [handler, this](const QJsonObject &json, QMcpJSONRPCErrorError *error) -> QJsonObject {
            // Make sure to respect the specific protocol version
            // Use our protocol version enum directly
            QtMcp::ProtocolVersion versionToUse = protocolVersion();

            // If this is a response to initialize, check the protocol version in the response
            if (json.contains("params"_L1) && json.value("params"_L1).toObject().contains("protocolVersion"_L1)) {
                QString reqVersionStr = json.value("params"_L1).toObject().value("protocolVersion"_L1).toString();
                QtMcp::ProtocolVersion reqVersion = QtMcp::stringToProtocolVersion(reqVersionStr);
                if (supportedProtocolVersions().contains(reqVersion)) {
                    versionToUse = reqVersion;
                }
            }

            Req req;
            req.fromJsonObject(json, versionToUse);
            Res res = handler(req, error);
            return res.toJsonObject(versionToUse);
        };

        registerRequestHandler(Req().method(), wrapper);
    }

    template <typename T> struct NotificationHandlerTraits;

    template <typename C, typename Arg>
    struct NotificationHandlerTraits<void(C::*)(Arg) const> {
        using NotificationType = std::decay_t<Arg>;
    };

    template <typename Handler>
    void addNotificationHandler(Handler handler)
    {
        using Traits = NotificationHandlerTraits<decltype(&Handler::operator())>;
        using Notification = typename Traits::NotificationType;

        static_assert(std::is_base_of<QMcpNotification, Notification>::value,
                      "Notification type must inherit from QMcpNotification");

        auto wrapper = [handler, this](const QJsonObject &json) {
            // Make sure to respect the specific protocol version
            // Use the enum protocol version
            QtMcp::ProtocolVersion versionToUse = protocolVersion();

            // If this is a notification containing protocol version info
            if (json.contains("params"_L1) && json.value("params"_L1).toObject().contains("protocolVersion"_L1)) {
                QString notifVersionStr = json.value("params"_L1).toObject().value("protocolVersion"_L1).toString();
                QtMcp::ProtocolVersion notifVersion = QtMcp::stringToProtocolVersion(notifVersionStr);
                if (supportedProtocolVersions().contains(notifVersion)) {
                    versionToUse = notifVersion;
                }
            }

            Notification notification;
            notification.fromJsonObject(json, versionToUse);
            handler(notification);
        };

        registerNotificationHandler(Notification().method(), wrapper);
    }

public slots:
    /*!
        Sets the protocol version to use for communication.
        This should be called before sending initialization requests.

        \param version The protocol version enum value
        \return true if the version is supported, false otherwise
    */
    void setProtocolVersion(QtMcp::ProtocolVersion protocolVersion);

    /*!
        Starts the MCP client with the given arguments.

        \param args Command-line style arguments to pass to the backend (e.g., "--log-level=debug")
    */
    void start(const QString &args);

signals:
    /*!
        Emitted when the protocol version changes.
        \param protocolVersion The new protocol version enum value
    */
    void protocolVersionChanged(QtMcp::ProtocolVersion protocolVersion);

    /*!
        Emitted when the client has successfully started.
    */
    void started();

    /*!
        Emitted when an error occurs during client operation.
        \param errorString Description of the error
    */
    void errorOccurred(const QString &errorString);

    /*!
        Emitted when a raw JSON message is received from the server.
        \param object The received JSON message
    */
    void received(const QJsonObject &object);

private:
    void send(const QJsonObject &message, std::function<void(const QJsonObject &, const QJsonObject &)> callback = nullptr);
    void registerRequestHandler(const QString &method, std::function<QJsonObject(const QJsonObject &, QMcpJSONRPCErrorError *)>);
    void registerNotificationHandler(const QString &method, std::function<void(const QJsonObject &)>);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPCLIENT_H
