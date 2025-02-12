// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpclient.h"
#include <QtCore/qjsonobject.h>
#include <QtCore/private/qfactoryloader_p.h>

#include <QtMcpClient/qmcpclientbackendplugin.h>
#include <QtMcpClient/qmcpclientbackendinterface.h>
#include <QtMcpCommon>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, backendLoader,
                          (QMcpClientBackendPluginFactoryInterface_iid, "/mcpclientbackend"_L1, Qt::CaseInsensitive))

class QMcpClient::Private
{
public:
    Private(const QString &type, QMcpClient *parent)
        : q(parent)
    {
        backend = qLoadPlugin<QMcpClientBackendInterface, QMcpClientBackendPlugin>(backendLoader(), type);
        if (!backend) {
            qWarning() << type << "not found";
            qWarning() << "call QMcpClient::backends() to get a list of available backends";
            qWarning() << QMcpClient::backends();
            return;
        }

        connect(backend, &QMcpClientBackendInterface::started, q, &QMcpClient::started);
        connect(backend, &QMcpClientBackendInterface::errorOccurred, q, &QMcpClient::errorOccurred);
        connect(backend, &QMcpClientBackendInterface::received, q, [this](const QJsonObject &object) {
            if (object.contains("id"_L1)) {
                const auto id = object.value("id"_L1);
                if (object.contains("result"_L1)) {
                    if (callbacks.contains(id)) {
                        const auto result = object.value("result"_L1).toObject();
                        callbacks.take(id)(result, {});
                        return;
                    }
                } else if (object.contains("error"_L1)) {
                    if (callbacks.contains(id)) {
                        const auto error = object.value("error"_L1).toObject();
                        callbacks.take(id)({}, error);
                        return;
                    }
                }
            }
            if (object.contains("method"_L1)) {
                const auto method = object.value("method"_L1).toString();

                // request
                if (object.contains("id"_L1)) {
                    const auto id = object.value("id"_L1);
                    if (requestHandlers.contains(method)) {
                        const auto handler = requestHandlers.value(method);
                        QMcpJSONRPCErrorError error;
                        const auto result = handler(object, &error);
                        if (error.code() > 0) {
                            QMcpJSONRPCError response;
                            response.setId(id.toVariant());
                            response.setError(error);
                            q->send(response.toJsonObject());
                        } else {
                            q->send(result);
                        }
                    } else {
                        // Respond with error
                        QMcpJSONRPCError response;
                        response.setId(id.toVariant());
                        auto error = response.error();
                        error.setMessage("Server doesn't handle the request"_L1);
                        response.setError(error);
                        q->send(response.toJsonObject());
                    }
                    return;
                }

                if (notificationHandlers.contains(method)) {
                    const auto handlers = notificationHandlers.values(method);
                    for (auto &handler : handlers) {
                        handler(object);
                    }
                    return;
                }
            }

            qWarning() << "not handled" << object;
        });
    }

private:
    QMcpClient *q;
public:
    QMcpClientBackendInterface *backend = nullptr;
    QHash<QJsonValue, std::function<void(const QJsonObject &, const QJsonObject &)>> callbacks;
    QHash<QString, std::function<QJsonObject(const QJsonObject &, QMcpJSONRPCErrorError *)>> requestHandlers;
    QMultiHash<QString, std::function<void(const QJsonObject &)>> notificationHandlers;
};

QStringList QMcpClient::backends()
{
    return backendLoader()->keyMap().values();
}

QMcpClient::QMcpClient(const QString &backend, QObject *parent)
    : QObject(parent)
    , d(new Private(backend, this))
{}

QMcpClient::~QMcpClient() = default;

void QMcpClient::start(const QString &args)
{
    if (!d->backend) return;
    d->backend->start(args);
}

void QMcpClient::send(const QJsonObject &request, std::function<void(const QJsonObject &, const QJsonObject &)> callback)
{
    if (!d->backend) return;
    static int id = 0;
    if (request.contains("id"_L1) && request.value("id"_L1).isNull()) {
        auto request2 = request;
        request2.insert("id"_L1, id);

        if (callback)
            d->callbacks.insert(id, callback);
        id++;
        d->backend->send(request2);
    } else {
        d->backend->send(request);
    }
}

void QMcpClient::registerRequestHandler(const QString &method, std::function<QJsonObject(const QJsonObject &, QMcpJSONRPCErrorError *)> callback)
{
    qDebug() << method;
    d->requestHandlers.insert(method, callback);
}

void QMcpClient::registerNotificationHandler(const QString &method, std::function<void(const QJsonObject &)> callback)
{
    qDebug() << method;
    d->notificationHandlers.insert(method, callback);
}

QT_END_NAMESPACE
