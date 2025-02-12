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
    Private(const QString &type, QMcpServer *parent);

private:
    QMcpServer *q;
public:
    QMcpServerBackendInterface *backend = nullptr;
    QMcpServerCapabilities capabilities;
    QString instructions;
    QString protocolVersion = "2024-11-05"_L1;
    QHash<QUuid, QHash<QJsonValue, std::function<void(const QJsonObject &)>>> callbacks;
    QHash<QString, std::function<QJsonObject(const QUuid &, const QJsonObject&, QMcpJSONRPCErrorError *)>> requestHandlers;
    QMultiHash<QString, std::function<void(const QUuid &, const QJsonObject&)>> notificationHandlers;
    QHash<QUuid, bool> initialized;
    QHash<QUuid, QList<QPair<QMcpResource, QMcpReadResourceResultContents>>> resources;
    QMultiHash<QUuid, QUrl> subscriptions;
};

QMcpServer::Private::Private(const QString &type, QMcpServer *parent)
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
    connect(backend, &QMcpServerBackendInterface::newSessionStarted, q, [this](const QUuid &session) {
        initialized.insert(session, false);
    });
    connect(backend, &QMcpServerBackendInterface::received, q, [this](const QUuid &session, const QJsonObject &object) {
        // response
        if (object.contains("id"_L1)) {
            const auto id = object.value("id"_L1);
            if (object.contains("result"_L1)) {
                if (callbacks[session].contains(id)) {
                    const auto result = object.value("result"_L1).toObject();
                    callbacks[session].take(id)(result);
                    return;
                }
            } else if (object.contains("error"_L1)) {
                qWarning() << "TODO: error handling" << object;;
                if (callbacks[session].contains(id)) {
                    callbacks[session].take(id)({});
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
                    const auto result = handler(session, object, &error);
                    if (error.code() > 0) {
                        QMcpJSONRPCError response;
                        response.setId(id);
                        response.setError(error);
                        q->send(session, response.toJsonObject());
                    } else {
                        QMcpJSONRPCResponse response;
                        response.setId(id);
                        auto object = response.toJsonObject();
                        object.insert("result"_L1, result);
                        q->send(session, object);
                    }
                } else {
                    // Respond with error
                    QMcpJSONRPCError response;
                    response.setId(id.toVariant());
                    auto error = response.error();
                    error.setMessage("Server doesn't handle the request"_L1);
                    response.setError(error);
                    q->send(session, response.toJsonObject());
                }
                return;
            }

            // notification
            if (notificationHandlers.contains(method)) {
                const auto handlers = notificationHandlers.values(method);
                for (auto &handler : handlers) {
                    handler(session, object);
                }
                return;
            }
        }

        qWarning() << "not handled" << object;
    });
}

QStringList QMcpServer::backends()
{
    return backendLoader()->keyMap().values();
}

QMcpServer::QMcpServer(const QString &backend, QObject *parent)
    : QObject(parent)
    , d(new Private(backend, this))
{
    addRequestHandler([this](const QUuid &session, const QMcpInitializeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpInitializeResult result;
        if (d->initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Initialized"_L1);
            return result;
        }
        if (request.params().protocolVersion() != "2024-11-05"_L1) {
            error->setCode(20241105);
            error->setMessage("Protocol Version %1 is not supported"_L1.arg(request.params().protocolVersion()));
            return result;
        }
        result.setCapabilities(d->capabilities);
        result.setInstructions(d->instructions);
        auto serverInfo = result.serverInfo();
        serverInfo.setName(QCoreApplication::applicationName());
        serverInfo.setVersion(QCoreApplication::applicationVersion());
        result.setServerInfo(serverInfo);
        result.setProtocolVersion(d->protocolVersion);
        return result;
    });
    addNotificationHandler([this](const QUuid &session, const QMcpInitializedNotification &notification) {
        Q_UNUSED(notification);
        d->initialized[session] = true;
        emit initialized(session);
    });

    addRequestHandler([](const QUuid &session, const QMcpPingRequest &, QMcpJSONRPCErrorError *) {
        Q_UNUSED(session); // ping can be accepted even before initialization
        QMcpResult result;
        return result;
    });

    addRequestHandler([this](const QUuid &session, const QMcpListResourcesRequest &, QMcpJSONRPCErrorError *error) {
        QMcpListResourcesResult result;
        if (!d->initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }
        auto resources = result.resources();
        for (const auto &pair : d->resources.value(session)) {
            resources.append(pair.first);
        }
        result.setResources(resources);
        return result;
    });

    addRequestHandler([this](const QUuid &session, const QMcpReadResourceRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpReadResourceResult result;
        if (!d->initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }

        const auto params = request.params();
        const auto uri = params.uri();
        auto contents = result.contents();
        const auto resources = d->resources.value(session);
        for (const auto &pair : resources) {
            if (pair.first.uri() == uri) {
                contents.append(pair.second);
            }
        }
        result.setContents(contents);
        return result;
    });


    addRequestHandler([this](const QUuid &session, const QMcpListToolsRequest &, QMcpJSONRPCErrorError *error) {
        QMcpListToolsResult result;
        if (!d->initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }
        result.setTools(tools());
        return result;
    });

    addRequestHandler([this](const QUuid &session, const QMcpSubscribeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpResult result;
        if (!d->initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }

        const auto params = request.params();
        const auto uri = params.uri().toString();
        d->subscriptions.insert(session, uri);
        return result;
    });

    addRequestHandler([this](const QUuid &session, const QMcpUnsubscribeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpResult result;
        if (!d->initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }

        const auto params = request.params();
        const auto uri = params.uri().toString();
        d->subscriptions.remove(session, uri);
        return result;
    });

    addRequestHandler([this](const QUuid &session, const QMcpCallToolRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpCallToolResult result;
        if (!d->initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }

        const auto params = request.params();
        bool ok;
        auto contents = callTool(session, params.name(), params.arguments(), &ok);
        if (ok) {
            result.setContent(contents);
        }
        return result;
    });
}

QMcpServer::~QMcpServer() = default;

void QMcpServer::start(const QString &args)
{
    if (!d->backend) return;
    d->backend->start(args);
}

void QMcpServer::send(const QUuid &session, const QJsonObject &request, std::function<void(const QJsonObject &)> callback)
{
    if (!d->backend) return;
    static int id = 0;
    if (request.contains("id"_L1) && request.value("id"_L1).isNull()) {
        auto request2 = request;
        request2.insert("id"_L1, id);

        if (callback)
            d->callbacks[session].insert(id, callback);
        id++;
        d->backend->send(session, request2);
    } else {
        d->backend->send(session, request);
    }
}

void QMcpServer::registerRequestHandler(const QString &method, std::function<QJsonObject(const QUuid &, const QJsonObject &, QMcpJSONRPCErrorError *)> callback)
{
    d->requestHandlers.insert(method, callback);
}

void QMcpServer::registerNotificationHandler(const QString &method, std::function<void(const QUuid &, const QJsonObject &)> callback)
{
    d->notificationHandlers.insert(method, callback);
}

QMcpServerCapabilities QMcpServer::capabilities() const
{
    return d->capabilities;
}

void QMcpServer::setCapabilities(const QMcpServerCapabilities &capabilities)
{
    if (d->capabilities == capabilities) return;
    d->capabilities = capabilities;
    emit capabilitiesChanged(capabilities);
}

QString QMcpServer::instructions() const
{
    return d->instructions;
}

void QMcpServer::setInstructions(const QString &instructions)
{
    if (d->instructions == instructions) return;
    d->instructions = instructions;
    emit instructionsChanged(instructions);
}

QString QMcpServer::protocolVersion() const
{
    return d->protocolVersion;
}

void QMcpServer::setProtocolVersion(const QString &protocolVersion)
{
    if (d->protocolVersion == protocolVersion) return;
    d->protocolVersion = protocolVersion;
    emit protocolVersionChanged(protocolVersion);
}

bool QMcpServer::isInitialized(const QUuid &session) const
{
    return d->initialized.value(session, false);
}

void QMcpServer::notifyResourceUpdated(const QUuid &session, const QMcpResource &resource)
{
    const auto uri = resource.uri();
    if (d->subscriptions.contains(session, uri)) {
        QMcpResourceUpdatedNotification notification;
        auto params = notification.params();
        params.setUri(uri);
        notification.setParams(params);
        send(session, notification.toJsonObject());
    }
}

void QMcpServer::appendResource(const QUuid &session, const QMcpResource &resource, const QMcpReadResourceResultContents &content)
{
    d->resources[session].append(std::make_pair(resource, content));
}

void QMcpServer::insertResource(const QUuid &session, int index, const QMcpResource &resource, const QMcpReadResourceResultContents &content)
{
    d->resources[session].insert(index, std::make_pair(resource, content));
}

void QMcpServer::replaceResource(const QUuid &session, int index, const QMcpResource resource, const QMcpReadResourceResultContents &content)
{
    d->resources[session][index] = std::make_pair(resource, content);
    notifyResourceUpdated(session, resource);
}

void QMcpServer::removeResourceAt(const QUuid &session, int index)
{
    const auto resource = d->resources[session][index].first;
    const auto content = d->resources[session][index].second;
    d->resources[session].removeAt(index);
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
            QSet<QString> internalTypes { "QUuid"_L1 };
            QJsonObject object;
            if (mcpTypes.contains(type))
                object.insert("type"_L1, mcpTypes.value(type));
            else if (internalTypes.contains(type))
                continue;
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

QList<QMcpCallToolResultContent> QMcpServer::callTool(const QUuid &session, const QString &name, const QJsonObject &params, bool *ok)
{
    QList<QMcpCallToolResultContent> ret;
    bool found = false;
    const auto *mo = metaObject();
    for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
        const auto mm = mo->method(i);
        if (mm.name() != name.toUtf8())
            continue;

        auto checkParams = [](const QStringList &a, QByteArrayList b) {
            QByteArrayList c;
            for (const auto &s : a)
                c.append(s.toUtf8());
            std::sort(b.begin(), b.end(), std::less<QByteArray>());
            std::sort(c.begin(), c.end(), std::less<QByteArray>());
            return b == c;
        };

        const auto names = mm.parameterNames();
        switch (mm.parameterCount() - params.size()) {
        case 0: // exact match
            if (!checkParams(params.keys(), names))
                continue;
            break;
        case 1: // may contain session
            if (!checkParams(params.keys() << "session"_L1, names))
                continue;
            break;
        default:
            continue;
        }

        QVariantList convertedArgs;
        convertedArgs.reserve(mm.parameterCount());

        const auto types = mm.parameterTypes();
        for (int j = 0; j < mm.parameterCount(); j++) {
            const auto type = types.at(j);
            const auto name = QString::fromUtf8(names.at(j));

            const auto metaType = QMetaType::fromName(type);
            switch (metaType.id()) {
            case QMetaType::UnknownType:
                qWarning() << "Unknown or unsupported type:" << type;
                break;
            case QMetaType::QUuid:
                convertedArgs.append(session);
                continue;
            default: {
                auto value = params.value(name).toVariant();
                if (!value.convert(mm.parameterMetaType(j))) {
                    qWarning() << "Failed to convert JSON value to type:" << type;
                    break;
                }
                convertedArgs.append(value);
                break; }
            }

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
            found = true;
            return ret; }
        case QMetaType::QString: {
            found = true;
            QString text = callMethod<QString>(this, &mm, convertedArgs);
            ret.append(QMcpTextContent(text));
            break; }
#ifdef QT_GUI_LIB
        case QMetaType::QImage: {
            found = true;
            QImage image = callMethod<QImage>(this, &mm, convertedArgs);
            ret.append(QMcpImageContent(image));
            break; }
#endif // QT_GUI_LIB
        default:
            qFatal() << mm.returnMetaType() << "not supported yet";
        }
    }
    if (ok)
        *ok = found;
    if (!found)
        qWarning() << name << "not found for " << params;
    return ret;
}

QHash<QString, QString> QMcpServer::descriptions() const
{
    return {};
}

QT_END_NAMESPACE
