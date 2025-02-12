// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVER_H
#define QMCPSERVER_H

#include <QtCore/QObject>
#include <QtMcpCommon/QMcpCallToolResultContent>
#include <QtMcpCommon/QMcpJSONRPCErrorError>
#include <QtMcpCommon/QMcpNotification>
#include <QtMcpCommon/QMcpReadResourceResultContents>
#include <QtMcpCommon/QMcpRequest>
#include <QtMcpCommon/QMcpResource>
#include <QtMcpCommon/QMcpResult>
#include <QtMcpCommon/QMcpServerCapabilities>
#include <QtMcpCommon/QMcpTool>
#include <QtMcpServer/qmcpserverglobal.h>
#include <concepts>
#include <functional>

QT_BEGIN_NAMESPACE

class Q_MCPSERVER_EXPORT QMcpServer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMcpServerCapabilities capabilities READ capabilities WRITE setCapabilities NOTIFY capabilitiesChanged FINAL)
    Q_PROPERTY(QString instructions READ instructions WRITE setInstructions NOTIFY instructionsChanged FINAL)
    Q_PROPERTY(QString protocolVersion READ protocolVersion WRITE setProtocolVersion NOTIFY protocolVersionChanged FINAL)
public:
    static QStringList backends();

    explicit QMcpServer(const QString &backend, QObject *parent = nullptr);
    ~QMcpServer() override;

    template<typename> struct CallbackArg;

    template<typename T, typename Arg>
    struct CallbackArg<void(T::*)(const Arg &) const> {
        using type = Arg;
    };

    template<typename T>
    struct CallbackArg : CallbackArg<decltype(&T::operator())> {};

    template<typename Callback>
    using CallbackResult = typename CallbackArg<Callback>::type;

    template<typename Request, typename Callback>
        requires std::invocable<Callback, const CallbackResult<Callback>&>
    void request(const QUuid &session, const Request &request, Callback callback)
    {
        using Result = CallbackResult<Callback>;

        static_assert(std::is_base_of<QMcpRequest, Request>::value, "Request must inherit from QMcpRequest");
        static_assert(std::is_base_of<QMcpResult, Result>::value, "Result must inherit from QMcpResult");

        auto json = request.toJsonObject();
        send(json, [callback](const QJsonObject &json) {
            Result result;
            result.fromJsonObject(json);
            callback(result);
        });
    }

    template<typename Request>
    void request(const Request &request)
    {
        static_assert(std::is_base_of<QMcpRequest, Request>::value, "Request must inherit from QMcpRequest");

        auto json = request.toJsonObject();
        send(json);
    }

    template<typename Notification>
    void notify(const Notification &notification)
    {
        static_assert(std::is_base_of<QMcpNotification, Notification>::value, "Notification must inherit from QMcpNotification");

        auto json = notification.toJsonObject();
        send(json);
    }


    template <typename T> struct RequestHandlerTraits;

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
        using Res = typename Traits::ResultType;

        static_assert(std::is_base_of<QMcpRequest, Req>::value,
                      "Request type must inherit from QMcpRequest");
        static_assert(std::is_base_of<QMcpResult, Res>::value,
                      "Result type must inherit from QMcpResult");

        auto wrapper = [handler](const QUuid &session, const QJsonObject &json, QMcpJSONRPCErrorError *error) -> QJsonObject {
            Req req;
            req.fromJsonObject(json);
            Res res = handler(session, req, error);
            return res.toJsonObject();
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

    QMcpServerCapabilities capabilities() const;
    QString instructions() const;
    QString protocolVersion() const;
    bool isInitialized(const QUuid &session) const;

    QList<QMcpTool> tools() const;
    QList<QMcpCallToolResultContent> callTool(const QUuid &session, const QString &name, const QJsonObject &params, bool *ok = nullptr);
    virtual QHash<QString, QString> descriptions() const;

public slots:
    void setCapabilities(const QMcpServerCapabilities &capabilities);
    void setInstructions(const QString &instructions);
    void setProtocolVersion(const QString &protocolVersion);
    void start(const QString &args = QString());

protected slots:
    void appendResource(const QUuid &session, const QMcpResource &resource, const QMcpReadResourceResultContents &content);
    void insertResource(const QUuid &session, int index, const QMcpResource &resource, const QMcpReadResourceResultContents &content);
    void replaceResource(const QUuid &session, int index, const QMcpResource resource, const QMcpReadResourceResultContents &content);
    void removeResourceAt(const QUuid &session, int index);

signals:
    void capabilitiesChanged(const QMcpServerCapabilities &capabilities);
    void instructionsChanged(const QString &instructions);
    void protocolVersionChanged(const QString &protocolVersion);
    void started();
    void initialized(const QUuid &uuid);
    void received(const QUuid &session, const QJsonObject &object);
    void result(const QUuid &session, const QJsonObject &result);

private:
    void notifyResourceUpdated(const QUuid &session, const QMcpResource &resource);
    void send(const QUuid &session, const QJsonObject &message, std::function<void(const QJsonObject &)> callback = nullptr);
    void registerRequestHandler(const QString &method, std::function<QJsonObject(const QUuid &, const QJsonObject &, QMcpJSONRPCErrorError *)>);
    void registerNotificationHandler(const QString &method, std::function<void(const QUuid &, const QJsonObject &)>);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPSERVER_H
