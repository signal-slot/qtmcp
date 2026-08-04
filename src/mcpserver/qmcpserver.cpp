// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserver.h"
#include "qmcpserversession.h"
#include <algorithm>
#include <QtCore/QDateTime>
#include <QtCore/QMetaType>
#include <QtCore/QPromise>
#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/qjsonobject.h>
#ifdef QT_GUI_LIB
#include <QtGui/QAction>
#endif
#include <QtMcpCommon>
#include <QtMcpServer/qmcpserverbackendinterface.h>
#include <QtMcpServer/qmcpserverbackendplugin.h>
QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, backendLoader,
                          (QMcpServerBackendPluginFactoryInterface_iid, "/mcpserverbackend"_L1, Qt::CaseInsensitive))

// Methods a 2026-07-28 session must reject with -32601: ping and
// logging/setLevel were dropped, resources/(un)subscribe were replaced by
// subscriptions/listen.
static const QStringList &methodsRemovedIn2026_07_28()
{
    static const QStringList methods = {
        "ping"_L1,
        "logging/setLevel"_L1,
        "resources/subscribe"_L1,
        "resources/unsubscribe"_L1,
    };
    return methods;
}

class QMcpServer::Private
{
public:
    Private(const QString &type, QMcpServer *parent);

    QMcpServerSession *findSession(const QUuid &sessionId, bool isInitialized, QMcpJSONRPCErrorError *error = nullptr) const;
    void sendTaggedNotification(QMcpServerSession *session, const QMcpNotification &notification) const;
private:
    QMcpServer *q;
public:
    QMcpServerBackendInterface *backend = nullptr;
    QMcpServerCapabilities capabilities;
    QString instructions;
    QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest; // Default to latest version
    QList<QtMcp::ProtocolVersion> supportedVersions = {QtMcp::ProtocolVersion::v2024_11_05, QtMcp::ProtocolVersion::v2025_03_26, QtMcp::ProtocolVersion::v2025_06_18, QtMcp::ProtocolVersion::v2025_11_25, QtMcp::ProtocolVersion::v2026_07_28};
    QHash<QUuid, QHash<QJsonValue, std::function<void(const QUuid &session, const QJsonObject &)>>> callbacks;
    QHash<QString, std::function<QJsonValue(const QUuid &, const QJsonObject&, QMcpJSONRPCErrorError *)>> requestHandlers;
    QMultiHash<QString, std::function<void(const QUuid &, const QJsonObject&)>> notificationHandlers;
    QHash<QUuid, QMcpServerSession *> sessions;
    QHash<QObject *, QHash<QString, QString>> toolSets;

    // io.modelcontextprotocol/tasks extension
    struct TaskEntry {
        QUuid session;
        QMcpTaskStatus::QMcpTaskStatus status = QMcpTaskStatus::working;
        QString createdAt;
        QString lastUpdatedAt;
        QFuture<QMcpCallToolResult> future;
        QJsonObject result;
        QJsonObject inputResponses;
    };
    // Shared with the task futures' continuations: a continuation may fire
    // while the server is being destroyed (QObject cancels them from its
    // destructor, after ~Private already ran), so it must own the registry
    // rather than reach through the dangling d pointer.
    using TaskMap = QHash<QString, TaskEntry>;
    std::shared_ptr<TaskMap> tasks = std::make_shared<TaskMap>();
    bool tasksExtensionEnabled = false;
#ifdef QT_GUI_LIB
    QHash<QAction *, QString> actions;
#endif
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
    QMcpServerCapabilitiesTools tools;
    tools.setListChanged(true);
    capabilities.setTools(tools);

    backend = qLoadPlugin<QMcpServerBackendInterface, QMcpServerBackendPlugin>(backendLoader(), type);
    if (!backend) {
        qWarning() << type << "not found";
        qWarning() << "call QMcpServer::backends() to get a list of available backends";
        qWarning() << QMcpServer::backends();
        return;
    }

    backend->setParent(q);
    connect(backend, &QMcpServerBackendInterface::started, q, &QMcpServer::started);
    connect(backend, &QMcpServerBackendInterface::finished, q, &QMcpServer::finished);
    connect(backend, &QMcpServerBackendInterface::newSessionStarted, q, [this](const QUuid &sessionId) {
        auto session = new QMcpServerSession(sessionId, q);

        // register self as tool set if it inherits from QMcpServer
        if (q->metaObject() != &QMcpServer::staticMetaObject)
            session->registerToolSet(q, q->toolDescriptions());

        // register known tool set
        for (auto i = toolSets.cbegin(), end = toolSets.cend(); i != end; ++i)
            session->registerToolSet(i.key(), i.value());
#ifdef QT_GUI_LIB
        for (auto i = actions.cbegin(), end = actions.cend(); i != end; ++i)
            session->registerTool(i.key(), i.value());
#endif

        sessions.insert(sessionId, session);
        // On sessions before 2026-07-28 change notifications flow freely once
        // the session is initialized; since 2026-07-28 they only go to clients
        // that opted in via subscriptions/listen, tagged with the
        // subscription id.
        connect(session, &QMcpServerSession::resourceUpdated, q, [this, session](const QMcpResource &resource) {
            if (!session->isInitialized()) return;
            const auto uri = resource.uri();
            if (session->protocolVersion() >= QtMcp::ProtocolVersion::v2026_07_28) {
                if (!session->hasListenSubscriptions()
                    || !session->listenSubscriptions().resourceSubscriptions().contains(uri.toString()))
                    return;
                QMcpResourceUpdatedNotification notification;
                auto params = notification.params();
                params.setUri(uri);
                notification.setParams(params);
                sendTaggedNotification(session, notification);
                return;
            }
            if (session->isSubscribed(uri)) {
                QMcpResourceUpdatedNotification notification;
                auto params = notification.params();
                params.setUri(uri);
                notification.setParams(params);
                q->notify(session->sessionId(), notification, session->protocolVersion());
            }
        });
        connect(session, &QMcpServerSession::resourceListChanged, q, [this, session]() {
            if (!session->isInitialized()) return;
            QMcpResourceListChangedNotification notification;
            if (session->protocolVersion() >= QtMcp::ProtocolVersion::v2026_07_28) {
                if (session->hasListenSubscriptions() && session->listenSubscriptions().resourcesListChanged())
                    sendTaggedNotification(session, notification);
                return;
            }
            q->notify(session->sessionId(), notification, session->protocolVersion());
        });
        connect(session, &QMcpServerSession::promptListChanged, q, [this, session]() {
            if (!session->isInitialized()) return;
            QMcpPromptListChangedNotification notification;
            if (session->protocolVersion() >= QtMcp::ProtocolVersion::v2026_07_28) {
                if (session->hasListenSubscriptions() && session->listenSubscriptions().promptsListChanged())
                    sendTaggedNotification(session, notification);
                return;
            }
            q->notify(session->sessionId(), notification, session->protocolVersion());
        });
        connect(session, &QMcpServerSession::toolListChanged, q, [this, session]() {
            if (!session->isInitialized()) return;
            QMcpToolListChangedNotification notification;
            if (session->protocolVersion() >= QtMcp::ProtocolVersion::v2026_07_28) {
                if (session->hasListenSubscriptions() && session->listenSubscriptions().toolsListChanged())
                    sendTaggedNotification(session, notification);
                return;
            }
            q->notify(session->sessionId(), notification, session->protocolVersion());
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

            // Stateless lifecycle (2026-07-28): instead of an initialize
            // handshake, every request carries the protocol version in its
            // params _meta. The first such request initializes the session.
            const auto requestMeta = object.value("params"_L1).toObject().value("_meta"_L1).toObject();
            const auto metaVersion = requestMeta.value("io.modelcontextprotocol/protocolVersion"_L1).toString();
            if (!metaVersion.isEmpty()) {
                const auto version = QtMcp::stringToProtocolVersion(metaVersion);
                if (version >= QtMcp::ProtocolVersion::v2026_07_28 && q->isProtocolVersionSupported(version)) {
                    if (auto *sessionObj = sessions.value(session)) {
                        if (!sessionObj->isInitialized()) {
                            sessionObj->setProtocolVersion(version);
                            sessionObj->setInitialized(true);
                        }
                        // Stateless clients re-declare their capabilities,
                        // including extensions, on every request.
                        sessionObj->setClientCapabilitiesJson(
                            requestMeta.value("io.modelcontextprotocol/clientCapabilities"_L1).toObject());
                    }
                }
            }

            // request
            if (object.contains("id"_L1)) {
                const auto id = object.value("id"_L1);
                const auto sessionForMethod = sessions.value(session);
                if (sessionForMethod && sessionForMethod->protocolVersion() >= QtMcp::ProtocolVersion::v2026_07_28
                    && methodsRemovedIn2026_07_28().contains(method)) {
                    QMcpJSONRPCError response;
                    response.setId(id.toVariant());
                    auto error = response.error();
                    error.setCode(-32601);
                    error.setMessage("Method '%1' was removed in MCP 2026-07-28"_L1.arg(method));
                    response.setError(error);
                    q->send(session, response.toJsonObject(sessionForMethod->protocolVersion()));
                    return;
                }
                if (requestHandlers.contains(method)) {
                    const auto handler = requestHandlers.value(method);
                    // MRTR (2026-07-28): make the retry's inputResponses and
                    // requestState available to the handler; both are empty on
                    // a first attempt.
                    if (sessionForMethod && sessionForMethod->protocolVersion() >= QtMcp::ProtocolVersion::v2026_07_28) {
                        const auto params = object.value("params"_L1).toObject();
                        sessionForMethod->provideInputResponses(params.value("inputResponses"_L1).toObject(),
                                                                params.value("requestState"_L1));
                    }
                    QMcpJSONRPCErrorError error;
                    auto result = handler(session, object, &error);
                    // Substitute only for synchronous handlers: an async
                    // handler returns an empty value here and its future
                    // continuation performs the same substitution when it
                    // sends the response.
                    if (result.isObject()) {
                        const auto interim = q->takePendingResultOverride(session);
                        if (!interim.isEmpty())
                            result = interim;
                    }
                    // JSON-RPC error codes are negative; any non-zero code
                    // set by the handler is an error.
                    if (error.code() != 0) {
                        QMcpJSONRPCError response;
                        response.setId(id);
                        response.setError(error);
                        auto sessionObj = sessions.value(session);
                        q->send(session, response.toJsonObject(sessionObj ?
                                sessionObj->protocolVersion() :
                                protocolVersion));
                    } else if (result.isObject()){
                        QMcpJSONRPCResponse response;
                        response.setId(id);
                        auto sessionObj = sessions.value(session);
                        const auto version = sessionObj ? sessionObj->protocolVersion() : protocolVersion;
                        auto object = response.toJsonObject(version);
                        auto resultObject = result.toObject();
                        if (version >= QtMcp::ProtocolVersion::v2026_07_28) {
                            // Since 2026-07-28 the server identifies itself in
                            // every result instead of only in initialize.
                            auto resultMeta = resultObject.value("_meta"_L1).toObject();
                            QJsonObject serverInfo;
                            serverInfo.insert("name"_L1, QCoreApplication::applicationName());
                            serverInfo.insert("version"_L1, QCoreApplication::applicationVersion());
                            resultMeta.insert("io.modelcontextprotocol/serverInfo"_L1, serverInfo);
                            resultObject.insert("_meta"_L1, resultMeta);
                        }
                        object.insert("result"_L1, resultObject);
                        q->send(session, object);
                    }
                } else {
                    // Respond with error
                    QMcpJSONRPCError response;
                    response.setId(id.toVariant());
                    auto error = response.error();
                    error.setMessage("Server doesn't handle the request"_L1);
                    response.setError(error);
                    auto sessionObj = sessions.value(session);
                    q->send(session, response.toJsonObject(sessionObj ?
                            sessionObj->protocolVersion() :
                            protocolVersion));
                }
                return;
            }

            // notification
            if (method == "notifications/roots/list_changed"_L1) {
                // Removed in 2026-07-28 together with the other
                // server-initiated request flows; ignore it there.
                const auto sessionObj = sessions.value(session);
                if (sessionObj && sessionObj->protocolVersion() >= QtMcp::ProtocolVersion::v2026_07_28) {
                    qWarning() << method << "was removed in MCP 2026-07-28, ignoring";
                    return;
                }
            }
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

// Sends a notification on a 2026-07-28 subscriptions/listen stream, tagged
// with the session's subscription id as the spec requires.
void QMcpServer::Private::sendTaggedNotification(QMcpServerSession *session, const QMcpNotification &notification) const
{
    auto object = notification.toJsonObject(session->protocolVersion());
    auto params = object.value("params"_L1).toObject();
    auto meta = params.value("_meta"_L1).toObject();
    meta.insert("io.modelcontextprotocol/subscriptionId"_L1, session->listenSubscriptionId());
    params.insert("_meta"_L1, meta);
    object.insert("params"_L1, params);
    q->send(session->sessionId(), object);
}

QMcpServerSession *QMcpServer::Private::findSession(const QUuid &sessionId, bool isInitialized, QMcpJSONRPCErrorError *error) const
{
    if (!sessions.contains(sessionId)) {
        if (error) {
            error->setCode(-32000);
            error->setMessage("No session found"_L1);
        }
        return nullptr;
    }
    auto session = sessions.value(sessionId);
    if (session->isInitialized() != isInitialized) {
        if (error) {
            error->setCode(-32000);
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

QJsonObject QMcpServer::takePendingResultOverride(const QUuid &session)
{
    auto *sessionObj = d->sessions.value(session);
    if (!sessionObj || sessionObj->protocolVersion() < QtMcp::ProtocolVersion::v2026_07_28)
        return QJsonObject();

    QJsonObject inputRequests;
    QJsonValue requestState;
    if (sessionObj->takeRequiredInput(&inputRequests, &requestState)) {
        QMcpInputRequiredResult result;
        result.setInputRequests(inputRequests);
        auto object = result.toJsonObject(sessionObj->protocolVersion());
        if (!requestState.isUndefined() && !requestState.isNull())
            object.insert("requestState"_L1, requestState);
        return object;
    }

    return sessionObj->takeResultOverride();
}

void QMcpServer::setTasksExtensionEnabled(bool enabled)
{
    d->tasksExtensionEnabled = enabled;
}

bool QMcpServer::isTasksExtensionEnabled() const
{
    return d->tasksExtensionEnabled;
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

        QtMcp::ProtocolVersion requestedVersion = request.params().protocolVersion();
        QtMcp::ProtocolVersion negotiatedVersion;

        if (supportedProtocolVersions().contains(requestedVersion)) {
            negotiatedVersion = requestedVersion;
        } else {
            // The lifecycle spec requires responding with a version the server
            // supports instead of erroring; the client decides whether to
            // disconnect. Offer the newest supported version.
            const auto versions = supportedProtocolVersions();
            negotiatedVersion = *std::max_element(versions.cbegin(), versions.cend());
        }

        // Store the negotiated version in the session
        if (session->protocolVersion() != negotiatedVersion) {
            const_cast<QMcpServerSession*>(session)->setProtocolVersion(negotiatedVersion);
        }

        result.setCapabilities(d->capabilities);
        result.setInstructions(d->instructions);
        auto serverInfo = result.serverInfo();
        serverInfo.setName(QCoreApplication::applicationName());
        serverInfo.setVersion(QCoreApplication::applicationVersion());
        result.setServerInfo(serverInfo);
        result.setProtocolVersion(QtMcp::protocolVersionToString(negotiatedVersion)); // Use the negotiated version
        return result;
    });
    addNotificationHandler([this](const QUuid &sessionId, const QMcpInitializedNotification &notification) {
        Q_UNUSED(notification);
        auto session = d->findSession(sessionId, false);
        if (!session)
            return;
        session->setInitialized(true);
    });
    // Mandatory since 2026-07-28, but answered on every session: it is the
    // version probe a client may send before knowing what the server speaks.
    addRequestHandler([this](const QUuid &sessionId, const QMcpDiscoverRequest &request, QMcpJSONRPCErrorError *error) {
        Q_UNUSED(sessionId);
        Q_UNUSED(request);
        Q_UNUSED(error);
        QMcpDiscoverResult result;
        QList<QString> versions;
        const auto supported = supportedProtocolVersions();
        for (const auto version : supported)
            versions.append(QtMcp::protocolVersionToString(version));
        result.setSupportedVersions(versions);
        auto capabilities = d->capabilities;
        if (d->tasksExtensionEnabled) {
            auto extensions = capabilities.extensions();
            extensions.insert("io.modelcontextprotocol/tasks"_L1, QJsonObject());
            capabilities.setExtensions(extensions);
        }
        result.setCapabilities(capabilities);
        result.setInstructions(d->instructions);
        return result;
    });
    addRequestHandler([this](const QUuid &sessionId, const QMcpSubscriptionsListenRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpSubscriptionsListenResult result;
        auto session = d->findSession(sessionId, true, error);
        if (!session)
            return result;
        session->setListenSubscriptions(request.params().notifications());

        QMcpSubscriptionsAcknowledgedNotification acknowledged;
        auto params = acknowledged.params();
        params.setNotifications(session->listenSubscriptions());
        acknowledged.setParams(params);
        d->sendTaggedNotification(session, acknowledged);
        return result;
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

    addRequestHandler([this](const QUuid &sessionId, const QMcpCallToolRequest &request, QMcpJSONRPCErrorError *error) -> QFuture<QMcpCallToolResult> {
        auto session = d->findSession(sessionId, true, error);
        if (!session) {
            QPromise<QMcpCallToolResult> promise;
            promise.start();
            promise.addResult(QMcpCallToolResult());
            promise.finish();
            return promise.future();
        }
        const auto params = request.params();
        const auto progressToken = params.meta().progressToken();
        auto future = session->callToolAsync(params.name(), params.arguments(), progressToken);

        // tasks extension: when both sides declared it and the tool has not
        // finished synchronously, hand out a durable task instead of keeping
        // the request pending.
        const bool clientWantsTasks = session->clientCapabilitiesJson()
            .value("extensions"_L1).toObject().contains("io.modelcontextprotocol/tasks"_L1);
        if (d->tasksExtensionEnabled && clientWantsTasks && !future.isFinished()) {
            const auto taskId = QUuid::createUuid().toString(QUuid::WithoutBraces);
            const auto now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
            Private::TaskEntry entry;
            entry.session = sessionId;
            entry.createdAt = now;
            entry.lastUpdatedAt = now;
            entry.future = future;
            d->tasks->insert(taskId, entry);
            const auto version = session->protocolVersion();
            auto tasks = d->tasks;
            future.then(this, [tasks, taskId, version](const QMcpCallToolResult &result) {
                auto &entry = (*tasks)[taskId];
                entry.status = QMcpTaskStatus::completed;
                entry.lastUpdatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
                entry.result = result.toJsonObject(version);
            }).onCanceled(this, [tasks, taskId]() {
                auto &entry = (*tasks)[taskId];
                entry.status = QMcpTaskStatus::cancelled;
                entry.lastUpdatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
            });

            QMcpExtCreateTaskResult createTask;
            createTask.setTaskId(taskId);
            createTask.setStatus(QMcpTaskStatus::working);
            createTask.setCreatedAt(now);
            createTask.setLastUpdatedAt(now);
            createTask.setTtlMs(300000);
            createTask.setPollIntervalMs(500);
            session->overrideResult(createTask.toJsonObject(version));

            // The handler still must return a future; hand back a finished
            // placeholder, the override above is what reaches the client.
            QPromise<QMcpCallToolResult> promise;
            promise.start();
            promise.addResult(QMcpCallToolResult());
            promise.finish();
            return promise.future();
        }
        return future;
    });
    registerRequestHandler("tasks/get"_L1, [this](const QUuid &sessionId, const QJsonObject &object, QMcpJSONRPCErrorError *error) -> QJsonValue {
        const auto taskId = object.value("params"_L1).toObject().value("taskId"_L1).toString();
        if (!d->tasksExtensionEnabled || !d->tasks->contains(taskId)) {
            error->setCode(-32602);
            error->setMessage("Unknown task '%1'"_L1.arg(taskId));
            return QJsonValue();
        }
        const auto entry = d->tasks->value(taskId);
        QMcpExtGetTaskResult result;
        result.setTaskId(taskId);
        result.setStatus(entry.status);
        result.setCreatedAt(entry.createdAt);
        result.setLastUpdatedAt(entry.lastUpdatedAt);
        result.setTtlMs(300000);
        result.setPollIntervalMs(500);
        if (entry.status == QMcpTaskStatus::completed)
            result.setResult(entry.result);
        return result.toJsonObject(versionToUse(sessionId));
    });
    registerRequestHandler("tasks/cancel"_L1, [this](const QUuid &sessionId, const QJsonObject &object, QMcpJSONRPCErrorError *error) -> QJsonValue {
        Q_UNUSED(sessionId);
        const auto taskId = object.value("params"_L1).toObject().value("taskId"_L1).toString();
        if (!d->tasksExtensionEnabled || !d->tasks->contains(taskId)) {
            error->setCode(-32602);
            error->setMessage("Unknown task '%1'"_L1.arg(taskId));
            return QJsonValue();
        }
        auto &entry = (*d->tasks)[taskId];
        // Cancelling the continuation does not reach the producing tool
        // (QFuture::cancel() does not propagate upstream), so the tool may
        // run to completion; its result is discarded and the task stays
        // cancelled, which is the cooperative semantics the extension allows.
        if (entry.status == QMcpTaskStatus::working)
            entry.future.cancel();
        QJsonObject result;
        result.insert("resultType"_L1, "complete"_L1);
        return result;
    });
    registerRequestHandler("tasks/update"_L1, [this](const QUuid &sessionId, const QJsonObject &object, QMcpJSONRPCErrorError *error) -> QJsonValue {
        Q_UNUSED(sessionId);
        const auto params = object.value("params"_L1).toObject();
        const auto taskId = params.value("taskId"_L1).toString();
        if (!d->tasksExtensionEnabled || !d->tasks->contains(taskId)) {
            error->setCode(-32602);
            error->setMessage("Unknown task '%1'"_L1.arg(taskId));
            return QJsonValue();
        }
        auto &entry = (*d->tasks)[taskId];
        // Stored for tools that requested input mid-task; wiring the
        // responses back into a suspended tool is an application concern for
        // now (the entry keeps the latest responses).
        entry.inputResponses = params.value("inputResponses"_L1).toObject();
        entry.lastUpdatedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        if (entry.status == QMcpTaskStatus::input_required)
            entry.status = QMcpTaskStatus::working;
        QJsonObject result;
        result.insert("resultType"_L1, "complete"_L1);
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
    const auto sessions = d->sessions.values();
    for (auto *session : sessions) {
        session->unregisterToolSet(toolSet);
    }
    d->toolSets.remove(toolSet);
}

#ifdef QT_GUI_LIB
void QMcpServer::registerTool(QAction *action, const QString &name)
{
    QString name2 = name.isEmpty() ? action->text() : name;
    d->actions.insert(action, name2);
    const auto sessions = d->sessions.values();
    for (auto *session : sessions) {
        session->registerTool(action, name2);
    }
}

void QMcpServer::unregisterTool(QAction *action)
{
    const auto sessions = d->sessions.values();
    for (auto *session : sessions) {
        session->unregisterTool(action);
    }
    d->actions.remove(action);
}
#endif

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

QtMcp::ProtocolVersion QMcpServer::protocolVersion() const
{
    return d->protocolVersion;
}

void QMcpServer::setProtocolVersion(QtMcp::ProtocolVersion protocolVersion)
{
    if (d->protocolVersion == protocolVersion) return;
    d->protocolVersion = protocolVersion;
    emit protocolVersionChanged(protocolVersion);
}


QList<QtMcp::ProtocolVersion> QMcpServer::supportedProtocolVersions() const
{
    return d->supportedVersions;
}

void QMcpServer::setSupportedProtocolVersions(const QList<QtMcp::ProtocolVersion> &versions)
{
    if (d->supportedVersions == versions) return;
    d->supportedVersions = versions;
    emit supportedProtocolVersionsChanged(versions);
}

bool QMcpServer::isProtocolVersionSupported(QtMcp::ProtocolVersion version) const
{
    return d->supportedVersions.contains(version);
}

QtMcp::ProtocolVersion QMcpServer::versionToUse(const QUuid &session, QtMcp::ProtocolVersion defaultVersion) const
{
    // The version negotiated for the session is authoritative: messages on a
    // session must always use the format the peer agreed to, so it takes
    // precedence over any caller-supplied default.
    const auto sessions = this->sessions();
    for (const auto *s : sessions) {
        if (s->sessionId() == session) {
            return s->protocolVersion();
        }
    }

    // No such session: honor an explicit override, then the server default
    if (defaultVersion != QtMcp::ProtocolVersion::Latest) {
        return defaultVersion;
    }
    return protocolVersion();
}

QHash<QString, QString> QMcpServer::toolDescriptions() const
{
    return {};
}

QList<QMcpServerSession *> QMcpServer::sessions() const
{
    return d->sessions.values();
}


QT_END_NAMESPACE
