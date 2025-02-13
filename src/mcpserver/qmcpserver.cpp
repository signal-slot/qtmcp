// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserver.h"
#include "qmcpserversession.h"
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

    QMcpServerSession *findSession(const QUuid &sessionId, bool isInitialized, QMcpJSONRPCErrorError *error = nullptr) const;
private:
    QMcpServer *q;
public:
    QMcpServerBackendInterface *backend = nullptr;
    QMcpServerCapabilities capabilities;
    QString instructions;
    QString protocolVersion = "2024-11-05"_L1;
    QHash<QUuid, QHash<QJsonValue, std::function<void(const QUuid &session, const QJsonObject &)>>> callbacks;
    QHash<QString, std::function<QJsonValue(const QUuid &, const QJsonObject&, QMcpJSONRPCErrorError *)>> requestHandlers;
    QMultiHash<QString, std::function<void(const QUuid &, const QJsonObject&)>> notificationHandlers;
    QHash<QUuid, QMcpServerSession *> sessions;
    QHash<QObject *, QHash<QString, QString>> toolSets;
};

QMcpServer::Private::Private(const QString &type, QMcpServer *parent)
    : q(parent)
{
    QMcpServerCapabilitiesResources resources;
    resources.setListChanged(true);
    resources.setSubscribe(true);
    capabilities.setResources(resources);
    QMcpServerCapabilitiesPrompts prompts;
    prompts.setListChanged(true);
    capabilities.setPrompts(prompts);
    // Tools are statically defined in subclasses as invokable methods
    // QMcpServerCapabilitiesTools tools;
    // tools.setListChanged(true);
    // capabilities.setTools(tools);

    backend = qLoadPlugin<QMcpServerBackendInterface, QMcpServerBackendPlugin>(backendLoader(), type);
    if (!backend) {
        qWarning() << type << "not found";
        qWarning() << "call QMcpServer::backends() to get a list of available backends";
        qWarning() << QMcpServer::backends();
        return;
    }

    connect(backend, &QMcpServerBackendInterface::started, q, &QMcpServer::started);
    connect(backend, &QMcpServerBackendInterface::newSessionStarted, q, [this](const QUuid &sessionId) {
        auto session = new QMcpServerSession(sessionId, q);

        // register self as tool set if it inherits from QMcpServer
        if (q->metaObject() != &QMcpServer::staticMetaObject)
            session->registerToolSet(q, q->toolDescriptions());

        // register known tool set
        for (auto i = toolSets.cbegin(), end = toolSets.cend(); i != end; ++i)
            session->registerToolSet(i.key(), i.value());

        sessions.insert(sessionId, session);
        connect(session, &QMcpServerSession::resourceUpdated, q, [this, session](const QMcpResource &resource) {
            const auto uri = resource.uri();
            if (session->isSubscribed(uri)) {
                QMcpResourceUpdatedNotification notification;
                auto params = notification.params();
                params.setUri(uri);
                notification.setParams(params);
                q->notify(session->sessionId(), notification);
            }

        });
        connect(session, &QMcpServerSession::resourceListChanged, q, [this, session]() {
            QMcpResourceListChangedNotification notification;
            q->notify(session->sessionId(), notification);
        });
        connect(session, &QMcpServerSession::promptListChanged, q, [this, session]() {
            QMcpPromptListChangedNotification notification;
            q->notify(session->sessionId(), notification);
        });
        connect(session, &QMcpServerSession::toolListChanged, q, [this, session]() {
            QMcpToolListChangedNotification notification;
            q->notify(session->sessionId(), notification);
        });

        emit q->newSession(session);
    });
    connect(backend, &QMcpServerBackendInterface::received, q, [this](const QUuid &session, const QJsonObject &object) {
        // response
        if (object.contains("id"_L1)) {
            const auto id = object.value("id"_L1);
            if (object.contains("result"_L1)) {
                if (callbacks[session].contains(id)) {
                    const auto result = object.value("result"_L1).toObject();
                    callbacks[session].take(id)(session, result);
                    return;
                }
            } else if (object.contains("error"_L1)) {
                qWarning() << "TODO: error handling" << object;;
                if (callbacks[session].contains(id)) {
                    callbacks[session].take(id)(session, {});
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
                    } else if (result.isObject()){
                        QMcpJSONRPCResponse response;
                        response.setId(id);
                        auto object = response.toJsonObject();
                        object.insert("result"_L1, result.toObject());
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

QMcpServerSession *QMcpServer::Private::findSession(const QUuid &sessionId, bool isInitialized, QMcpJSONRPCErrorError *error) const
{
    if (!sessions.contains(sessionId)) {
        if (error) {
            error->setCode(1);
            error->setMessage("No session found"_L1);
        }
        return nullptr;
    }
    auto session = sessions.value(sessionId);
    if (session->isInitialized() != isInitialized) {
        if (error) {
            error->setCode(1);
            error->setMessage("Initialized state mismatch"_L1);
        }
        return nullptr;
    }
    return session;
}

QStringList QMcpServer::backends()
{
    return backendLoader()->keyMap().values();
}

QMcpServer::QMcpServer(const QString &backend, QObject *parent)
    : QObject(parent)
    , d(new Private(backend, this))
{
    addRequestHandler([this](const QUuid &sessionId, const QMcpInitializeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpInitializeResult result;
        auto session = d->findSession(sessionId, false, error);
        if (!session)
            return result;
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
    addNotificationHandler([this](const QUuid &sessionId, const QMcpInitializedNotification &notification) {
        Q_UNUSED(notification);
        auto session = d->findSession(sessionId, false);
        if (!session)
            return;
        session->setInitialized(true);
    });

    addRequestHandler([](const QUuid &session, const QMcpPingRequest &, QMcpJSONRPCErrorError *) {
        Q_UNUSED(session); // ping can be accepted even before initialization
        QMcpEmptyResult result;
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpListResourceTemplatesRequest &, QMcpJSONRPCErrorError *error) {
        QMcpListResourceTemplatesResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        result.setResourceTemplates(session->resourceTemplates());
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpListResourcesRequest &, QMcpJSONRPCErrorError *error) {
        QMcpListResourcesResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        result.setResources(session->resources());
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpReadResourceRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpReadResourceResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        const auto params = request.params();
        const auto uri = params.uri();
        result.setContents(session->contents(uri));
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpListToolsRequest &, QMcpJSONRPCErrorError *error) {
        QMcpListToolsResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        result.setTools(session->tools());
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpSubscribeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpEmptyResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        const auto params = request.params();
        const auto uri = params.uri().toString();
        session->subscribe(uri);
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpUnsubscribeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpEmptyResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        const auto params = request.params();
        const auto uri = params.uri().toString();
        session->unsubscribe(uri);
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpCallToolRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpCallToolResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        const auto params = request.params();
        bool ok;
        auto contents = session->callTool(params.name(), params.arguments(), &ok);
        if (ok) {
            result.setContent(contents);
        }
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpListPromptsRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpListPromptsResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        auto cursor = request.params().cursor();
        auto prompts = session->prompts(&cursor);
        result.setPrompts(prompts);
        result.setNextCursor(cursor);
        return result;
    });

    addRequestHandler([this](const QUuid &sessionId, const QMcpGetPromptRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpGetPromptResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        const auto params = request.params();
        const auto name = params.name();
        result.setMessages(session->messages(name));
        return result;
    });

    addNotificationHandler([this](const QUuid &sessionId, const QMcpRootsListChangedNotification &notification) {
        Q_UNUSED(notification);
        auto session = d->findSession(sessionId, true);
        if (!session)
            return;

        QMcpListRootsRequest request;
        this->request(sessionId, request, [this](const QUuid &sessionId, const QMcpListRootsResult &result) {
            auto session = d->findSession(sessionId, true);
            if (session)
                session->setRoots(result.roots());
        });
    });
}

QMcpServer::~QMcpServer() = default;

void QMcpServer::start(const QString &args)
{
    if (!d->backend) return;
    d->backend->start(args);
}

void QMcpServer::registerToolSet(QObject *toolSet, const QHash<QString, QString> &descriptions)
{
    d->toolSets.insert(toolSet, descriptions);
    const auto sessions = d->sessions.values();
    for (auto *session : sessions) {
        session->registerToolSet(toolSet, descriptions);
    }
}

void QMcpServer::unregisterToolSet(QObject *toolSet)
{
    d->toolSets.remove(toolSet);
    const auto sessions = d->sessions.values();
    for (auto *session : sessions) {
        session->unregisterToolSet(toolSet);
    }
}

void QMcpServer::send(const QUuid &session, const QJsonObject &request, std::function<void(const QUuid &session, const QJsonObject &)> callback)
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

void QMcpServer::registerRequestHandler(const QString &method, std::function<QJsonValue(const QUuid &, const QJsonObject &, QMcpJSONRPCErrorError *)> callback)
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

QHash<QString, QString> QMcpServer::toolDescriptions() const
{
    return {};
}

QT_END_NAMESPACE
