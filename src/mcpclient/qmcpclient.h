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
#include <concepts>
#include <functional>

QT_BEGIN_NAMESPACE

class Q_MCPCLIENT_EXPORT QMcpClient : public QObject
{
    Q_OBJECT
public:
    static QStringList backends();

    explicit QMcpClient(const QString &backend, QObject *parent = nullptr);
    ~QMcpClient() override;

    template<typename> struct CallbackArg;

    template<typename T, typename Arg>
    struct CallbackArg<void(T::*)(const Arg &, const QMcpJSONRPCErrorError *) const> {
        using type = Arg;
    };

    template<typename T>
    struct CallbackArg : CallbackArg<decltype(&T::operator())> {};

    template<typename Callback>
    using CallbackResult = typename CallbackArg<Callback>::type;

    template<typename Request, typename Callback>
        requires std::invocable<Callback, const CallbackResult<Callback> &, const QMcpJSONRPCErrorError *>
    void request(const Request &request, Callback callback)
    {
        using Result = CallbackResult<Callback>;

        static_assert(std::is_base_of<QMcpRequest, Request>::value, "Request must inherit from QMcpRequest");
        static_assert(std::is_base_of<QMcpResult, Result>::value, "Result must inherit from QMcpResult");

        auto json = request.toJsonObject();
        send(json, [callback](const QJsonObject &json, const QJsonObject &error) {
            Result result;
            result.fromJsonObject(json);
            if (!error.isEmpty()) {
                QMcpJSONRPCErrorError e;
                e.fromJsonObject(error);
                callback(result, &e);
            } else {
                callback(result, nullptr);
            }
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
    struct RequestHandlerTraits<R(C::*)(Arg, QMcpJSONRPCErrorError *) const> {
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

        auto wrapper = [handler](const QJsonObject &json, QMcpJSONRPCErrorError *error) -> QJsonObject {
            Req req;
            req.fromJsonObject(json);
            Res res = handler(req, error);
            return res.toJsonObject();
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

        auto wrapper = [handler](const QJsonObject &json) {
            Notification notification;
            notification.fromJsonObject(json);
            handler(notification);
        };

        registerNotificationHandler(Notification().method(), wrapper);
    }

public slots:
    void start(const QString &args);

signals:
    void started();
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
