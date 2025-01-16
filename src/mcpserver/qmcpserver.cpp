// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserver.h"
#include <QtCore/QMetaType>
#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/qjsonobject.h>
#ifdef QT_GUI_LIB
#include <QtGui/QImage>
#endif
#include <QtMcpCommon>
#include <QtMcpServer/qmcpserverbackendinterface.h>
#include <QtMcpServer/qmcpserverbackendplugin.h>
QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, backendLoader,
                          (QMcpServerBackendPluginFactoryInterface_iid, "/mcpserverbackend"_L1, Qt::CaseInsensitive))

class QMcpServer::Private
{
public:
    Private(const QString &type, QMcpServer *parent)
        : q(parent)
    {
        backend = qLoadPlugin<QMcpServerBackendInterface, QMcpServerBackendPlugin>(backendLoader(), type);
        if (!backend) {
            qWarning() << type << "not found";
            qWarning() << "call QMcpServer::backends() to get a list of available backends";
            qWarning() << QMcpServer::backends();
            return;
        }

        connect(backend, &QMcpServerBackendInterface::started, q, &QMcpServer::started);
        connect(backend, &QMcpServerBackendInterface::received, q, [this](const QJsonObject &object) {
            // response
            if (object.contains("id"_L1)) {
                const auto id = object.value("id"_L1);
                if (object.contains("result"_L1)) {
                    if (callbacks.contains(id)) {
                        const auto result = object.value("result"_L1).toObject();
                        callbacks.take(id)(result);
                        return;
                    }
                } else if (object.contains("error"_L1)) {
                    qWarning() << "TODO: error handling" << object;;
                    if (callbacks.contains(id)) {
                        callbacks.take(id)({});
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
                            response.setId(id);
                            response.setError(error);
                            q->send(response.toJsonObject());
                        } else {
                            QMcpJSONRPCResponse response;
                            response.setId(id);
                            auto object = response.toJsonObject();
                            object.insert("result"_L1, result);
                            q->send(object);
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

                // notification
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
    QMcpServer *q;
public:
    QMcpServerBackendInterface *backend = nullptr;
    QHash<QJsonValue, std::function<void(const QJsonObject &)>> callbacks;
    QHash<QString, std::function<QJsonObject(const QJsonObject&, QMcpJSONRPCErrorError *)>> requestHandlers;
    QMultiHash<QString, std::function<void(const QJsonObject&)>> notificationHandlers;
};

QStringList QMcpServer::backends()
{
    return backendLoader()->keyMap().values();
}

QMcpServer::QMcpServer(const QString &backend, QObject *parent)
    : QObject(parent)
    , d(new Private(backend, this))
{}

QMcpServer::~QMcpServer() = default;

void QMcpServer::start(const QString &args)
{
    if (!d->backend) return;
    d->backend->start(args);
}

void QMcpServer::send(const QJsonObject &request, std::function<void(const QJsonObject &)> callback)
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

void QMcpServer::registerRequestHandler(const QString &method, std::function<QJsonObject(const QJsonObject &, QMcpJSONRPCErrorError *)> callback)
{
    qDebug() << method;
    d->requestHandlers.insert(method, callback);
}

void QMcpServer::registerNotificationHandler(const QString &method, std::function<void(const QJsonObject &)> callback)
{
    qDebug() << method;
    d->notificationHandlers.insert(method, callback);
}

QList<QMcpTool> QMcpServer::tools() const
{
    QList<QMcpTool> ret;
    const auto *mo = metaObject();
    qDebug() << mo->className();

    for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
        const auto mm = mo->method(i);
        QMcpTool tool;
        const auto name = QString::fromUtf8(mm.name());
        tool.setName(name);
        if (descriptions().contains(name)) {
            tool.setDescription(descriptions().value(name));
        }
        QMcpToolInputSchema inputSchema;
        auto required = inputSchema.required();
        auto properties = inputSchema.properties();
        const auto types = mm.parameterTypes();
        const auto names = mm.parameterNames();
        for (int j = 0; j < mm.parameterCount(); j++) {
            const auto type = QString::fromUtf8(types.at(j));
            const auto name = QString::fromUtf8(names.at(j));
            // message
            QHash<QString, QString> mcpTypes {
                                             { "QString", "string" },
                                             { "int", "number" },
                                             };

            QJsonObject object;
            if (mcpTypes.contains(type))
                object.insert("type"_L1, mcpTypes.value(type));
            else
                qWarning() << "Unknown type" << type;

            if (descriptions().contains("%1/%2"_L1.arg(tool.name(), name))) {
                object.insert("description"_L1, descriptions().value("%1/%2"_L1.arg(tool.name(), name)));
            }
            properties.insert(name, object);
            required.append(name);
        }
        inputSchema.setProperties(properties);
        inputSchema.setRequired(required);
        tool.setInputSchema(inputSchema);
        ret.append(tool);
    }
    return ret;
}

namespace {
template<class T>
T callMethod(QObject *object, const QMetaMethod *method, const QVariantList &args)
{
    T result;
    QGenericReturnArgument ret(method->returnMetaType().name(), &result);
    switch (method->parameterCount()) {
    case 0:
        method->invoke(object,
                  ret
                  );
        break;
    case 1:
        method->invoke(object,
                  ret,
                  QGenericArgument(args.at(0).typeName(), args.at(0).constData())
                  );
        break;
    case 2:
        method->invoke(object,
                  ret,
                  QGenericArgument(args.at(0).typeName(), args.at(0).constData()),
                  QGenericArgument(args.at(1).typeName(), args.at(1).constData())
                  );
        break;
    case 3:
        method->invoke(object,
                  ret,
                  QGenericArgument(args.at(0).typeName(), args.at(0).constData()),
                  QGenericArgument(args.at(1).typeName(), args.at(1).constData()),
                  QGenericArgument(args.at(2).typeName(), args.at(2).constData())
                  );
        break;
    default:
        qFatal() << "callMethod: too many parameters, or not implemented in switch.";
    }
    return result;
}
}

QList<QMcpCallToolResultContent> QMcpServer::callTool(const QString &name, const QJsonObject &params, bool *ok)
{
    QList<QMcpCallToolResultContent> ret;
    if (ok)
        *ok = false;
    const auto *mo = metaObject();
    for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
        const auto mm = mo->method(i);
        if (mm.name() != name.toUtf8())
            continue;

        if (params.size() != mm.parameterCount())
            continue;

        const auto types = mm.parameterTypes();
        const auto names = mm.parameterNames();

        if (![](const QStringList &a, QByteArrayList b) {
                QByteArrayList c;
                for (const auto &s : a)
                    c.append(s.toUtf8());
                std::sort(b.begin(), b.end(), std::less<QByteArray>());
                std::sort(c.begin(), c.end(), std::less<QByteArray>());
            return b == c;
            }(params.keys(), names))
            continue;

        QVariantList convertedArgs;
        convertedArgs.reserve(mm.parameterCount());

        for (int j = 0; j < mm.parameterCount(); j++) {
            const auto type = types.at(j);
            const auto name = QString::fromUtf8(names.at(j));
            auto value = params.value(name).toVariant();

            const auto metaType = QMetaType::fromName(type);
            if (metaType.id() == QMetaType::UnknownType) {
                qWarning() << "Unknown or unsupported type:" << type;
                break;
            }

            if (!value.convert(mm.parameterMetaType(j))) {
                qWarning() << "Failed to convert JSON value to type:" << type;
                break;
            }

            convertedArgs.append(value);
        }

        if (convertedArgs.count() != mm.parameterCount())
            continue;

        switch (mm.returnMetaType().id()) {
        case QMetaType::Void: {
            switch (mm.parameterCount()) {
            case 0:
                mm.invoke(this,
                          Qt::DirectConnection
                          );
                break;
            case 1:
                mm.invoke(this,
                          Qt::DirectConnection,
                          QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData())
                          );
                break;
            case 2:
                mm.invoke(this,
                          Qt::DirectConnection,
                          QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()),
                          QGenericArgument(convertedArgs[1].typeName(), convertedArgs[1].constData())
                          );
                break;
            case 3:
                mm.invoke(this,
                          Qt::DirectConnection,
                          QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()),
                          QGenericArgument(convertedArgs[1].typeName(), convertedArgs[1].constData()),
                          QGenericArgument(convertedArgs[2].typeName(), convertedArgs[2].constData())
                          );
                break;
            default:
                qFatal() << "invokeMethodWithJson: too many parameters, or not implemented in switch.";
            }
            if (ok)
                *ok = true;
            return ret; }
        case QMetaType::QString: {
            if (ok)
                *ok = true;
            QString text = callMethod<QString>(this, &mm, convertedArgs);
            ret.append(QMcpTextContent(text));
            break; }
#ifdef QT_GUI_LIB
        case QMetaType::QImage: {
            if (ok)
                *ok = true;
            QImage image = callMethod<QImage>(this, &mm, convertedArgs);
            ret.append(QMcpImageContent(image));
            break; }
#endif // QT_GUI_LIB
        default:
            qFatal() << mm.returnMetaType() << "not supported yet";
        }
    }
    qWarning() << name << "not found for " << params;
    return ret;
}

QHash<QString, QString> QMcpServer::descriptions() const
{
    return {};
}

QT_END_NAMESPACE
