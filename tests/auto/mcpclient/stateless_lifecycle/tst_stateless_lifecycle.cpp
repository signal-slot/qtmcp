// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QFuture>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QPromise>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include <QtMcpClient/QMcpClient>
#include <QtMcpClient/QMcpClientBackendInterface>
#include <QtMcpCommon/QMcpCallToolResultContent>
#include <QtMcpCommon/QMcpDiscoverRequest>
#include <QtMcpCommon/QMcpDiscoverResult>
#include <QtMcpCommon/QMcpEmptyResult>
#include <QtMcpCommon/QMcpInitializeRequest>
#include <QtMcpCommon/QMcpInitializeResult>
#include <QtMcpCommon/QMcpInitializedNotification>
#include <QtMcpCommon/QMcpListToolsRequest>
#include <QtMcpCommon/QMcpListToolsResult>
#include <QtMcpCommon/QMcpPingRequest>
#include <QtMcpCommon/QMcpPrompt>
#include <QtMcpCommon/QMcpPromptMessage>
#include <QtMcpCommon/QMcpSetLevelRequest>
#include <QtMcpCommon/QMcpSubscribeRequest>
#include <QtMcpCommon/QMcpSubscriptionFilter>
#include <QtMcpCommon/QMcpSubscriptionsListenRequest>
#include <QtMcpCommon/QMcpSubscriptionsListenRequestParams>
#include <QtMcpCommon/QMcpSubscriptionsListenResult>
#include <QtMcpCommon/QMcpTextContent>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtMcpServer/QMcpServer>
#include <QtMcpServer/QMcpServerSession>

#include <memory>
#include <optional>

namespace {
// A tool set exists only so that registering it makes the session report a
// changed tool list.
class ToolSet : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    Q_INVOKABLE QFuture<QList<QMcpCallToolResultContent>> echo(const QString &message)
    {
        QPromise<QList<QMcpCallToolResultContent>> promise;
        promise.start();
        promise.addResult({ QMcpTextContent(message) });
        promise.finish();
        return promise.future();
    }
};

// The state a request callback fills in. It is held by shared_ptr so that a
// late answer, arriving after the waiting helper gave up, cannot write to a
// destroyed stack frame.
struct CallState
{
    bool answered = false;
    std::optional<int> errorCode;
};

// Sends \a request and waits for its answer. Returns the shared state, whose
// answered member tells whether an answer arrived at all and whose errorCode
// holds the JSON-RPC error code when the server refused the request.
template<typename Result, typename Request>
std::shared_ptr<CallState> call(QMcpClient *client, const Request &request)
{
    auto state = std::make_shared<CallState>();
    client->request(request, [state](const Result &, const QMcpJSONRPCErrorError *error) {
        state->answered = true;
        if (error)
            state->errorCode = error->code();
    });
    QTest::qWaitFor([state] { return state->answered; }, 5000);
    return state;
}

QString subscriptionIdOf(const QJsonObject &notification)
{
    return notification.value("params"_L1).toObject()
        .value("_meta"_L1).toObject()
        .value("io.modelcontextprotocol/subscriptionId"_L1).toString();
}
} // namespace

class tst_StatelessLifecycle : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void toolsListInitializesTheSession();
    void discoverAdvertisesTheSupportedVersions();
    void removedMethodsAreRejected();
    void removedMethodsStillWorkOnOlderSessions();
    void notificationsNeedAnOptIn();

private:
    QMcpServer *m_server = nullptr;
    QMcpClient *m_client = nullptr;
    // Every message the transport handed to the client, so that the tests can
    // assert on the _meta members the typed API does not expose.
    QList<QJsonObject> m_received;

    QMcpServerSession *session() const;
    QJsonObject receivedNotification(const QString &method) const;
    QJsonObject receivedResultContaining(const QString &key) const;
    bool initializeSession(QtMcp::ProtocolVersion version);
};

void tst_StatelessLifecycle::init()
{
    m_received.clear();

    // The server keeps its default supported version list, which covers every
    // revision including 2026-07-28.
    m_server = new QMcpServer("sse"_L1, this);
    m_server->start("127.0.0.1:10102"_L1);

    m_client = new QMcpClient("sse"_L1, this);

    QSignalSpy startedSpy(m_client, &QMcpClient::started);
    m_client->start("http://localhost:10102"_L1);

    // The transport is the only place where the raw JSON is still available;
    // QMcpClient hands the tests typed gadgets that drop unknown _meta members.
    auto *backend = m_client->findChild<QMcpClientBackendInterface *>();
    QVERIFY(backend);
    connect(backend, &QMcpClientBackendInterface::received, this, [this](const QJsonObject &object) {
        m_received.append(object);
    });

    QVERIFY(startedSpy.wait(5000));
    QVERIFY(session());
}

void tst_StatelessLifecycle::cleanup()
{
    delete m_client;
    m_client = nullptr;
    delete m_server;
    m_server = nullptr;
}

QMcpServerSession *tst_StatelessLifecycle::session() const
{
    const auto sessions = m_server->sessions();
    if (sessions.isEmpty())
        return nullptr;
    return sessions.first();
}

QJsonObject tst_StatelessLifecycle::receivedNotification(const QString &method) const
{
    for (const auto &object : m_received) {
        if (!object.contains("id"_L1) && object.value("method"_L1).toString() == method)
            return object;
    }
    return {};
}

QJsonObject tst_StatelessLifecycle::receivedResultContaining(const QString &key) const
{
    for (const auto &object : m_received) {
        const auto result = object.value("result"_L1).toObject();
        if (result.contains(key))
            return result;
    }
    return {};
}

bool tst_StatelessLifecycle::initializeSession(QtMcp::ProtocolVersion version)
{
    m_client->setProtocolVersion(version);

    QMcpInitializeRequest request;
    auto params = request.params();
    params.setProtocolVersion(QtMcp::protocolVersionToString(version));
    request.setParams(params);

    auto state = std::make_shared<CallState>();
    m_client->request(request, [state](const QMcpInitializeResult &, const QMcpJSONRPCErrorError *error) {
        state->answered = true;
        if (error)
            state->errorCode = error->code();
    });
    if (!QTest::qWaitFor([state] { return state->answered; }, 5000) || state->errorCode)
        return false;

    // A session only counts as initialized once the client says it is done.
    m_client->notify(QMcpInitializedNotification());
    return QTest::qWaitFor([this] {
        auto *serverSession = session();
        return serverSession && serverSession->isInitialized();
    }, 5000);
}

void tst_StatelessLifecycle::toolsListInitializesTheSession()
{
    auto *serverSession = session();
    QVERIFY(!serverSession->isInitialized());
    // Put the session on an older revision first, so that the version the
    // request carries is provably what the server picks up, rather than the
    // default a fresh session happens to start with.
    serverSession->setProtocolVersion(QtMcp::ProtocolVersion::v2025_11_25);

    m_client->setProtocolVersion(QtMcp::ProtocolVersion::v2026_07_28);

    // No initialize handshake happens here: since 2026-07-28 every request
    // carries the protocol version in its params _meta instead.
    auto answer = std::make_shared<CallState>();
    auto tools = std::make_shared<std::optional<QMcpListToolsResult>>();
    m_client->request(QMcpListToolsRequest(),
                      [answer, tools](const QMcpListToolsResult &result, const QMcpJSONRPCErrorError *error) {
        answer->answered = true;
        if (error)
            answer->errorCode = error->code();
        else
            *tools = result;
    });
    QVERIFY(QTest::qWaitFor([answer] { return answer->answered; }, 5000));
    QVERIFY(!answer->errorCode);
    QVERIFY(tools->has_value());
    QCOMPARE(tools->value().resultType(), "complete"_L1);

    // The request initialized the session on the revision it announced.
    QVERIFY(serverSession->isInitialized());
    QCOMPARE(serverSession->protocolVersion(), QtMcp::ProtocolVersion::v2026_07_28);

    // Since 2026-07-28 the server identifies itself in every result instead of
    // only in the initialize result.
    const auto result = receivedResultContaining("tools"_L1);
    QVERIFY(!result.isEmpty());
    const auto serverInfo = result.value("_meta"_L1).toObject()
        .value("io.modelcontextprotocol/serverInfo"_L1).toObject();
    QVERIFY(!serverInfo.isEmpty());
    QVERIFY(!serverInfo.value("name"_L1).toString().isEmpty());
    QVERIFY(serverInfo.contains("version"_L1));
}

void tst_StatelessLifecycle::discoverAdvertisesTheSupportedVersions()
{
    m_client->setProtocolVersion(QtMcp::ProtocolVersion::v2026_07_28);

    auto answer = std::make_shared<CallState>();
    auto discovered = std::make_shared<std::optional<QMcpDiscoverResult>>();
    m_client->request(QMcpDiscoverRequest(),
                      [answer, discovered](const QMcpDiscoverResult &result, const QMcpJSONRPCErrorError *error) {
        answer->answered = true;
        if (error)
            answer->errorCode = error->code();
        else
            *discovered = result;
    });
    QVERIFY(QTest::qWaitFor([answer] { return answer->answered; }, 5000));
    QVERIFY(!answer->errorCode);
    QVERIFY(discovered->has_value());

    // server/discover is how a client learns what the server speaks before
    // committing to a revision, so the list must name every version the server
    // supports.
    const auto versions = discovered->value().supportedVersions();
    QVERIFY(versions.contains("2026-07-28"_L1));
    QVERIFY(versions.contains("2025-11-25"_L1));
    QVERIFY(versions.contains("2024-11-05"_L1));
    QCOMPARE(versions.size(), m_server->supportedProtocolVersions().size());
}

void tst_StatelessLifecycle::removedMethodsAreRejected()
{
    m_client->setProtocolVersion(QtMcp::ProtocolVersion::v2026_07_28);

    // ping and logging/setLevel were dropped in 2026-07-28, and
    // resources/subscribe was replaced by subscriptions/listen. A session on
    // that revision has to answer all of them with "method not found".
    const auto ping = call<QMcpEmptyResult>(m_client, QMcpPingRequest());
    QVERIFY(ping->answered);
    QCOMPARE(ping->errorCode, -32601);

    const auto setLevel = call<QMcpEmptyResult>(m_client, QMcpSetLevelRequest());
    QVERIFY(setLevel->answered);
    QCOMPARE(setLevel->errorCode, -32601);

    const auto subscribe = call<QMcpEmptyResult>(m_client, QMcpSubscribeRequest());
    QVERIFY(subscribe->answered);
    QCOMPARE(subscribe->errorCode, -32601);
}

void tst_StatelessLifecycle::removedMethodsStillWorkOnOlderSessions()
{
    // The removal is bound to the negotiated revision, not to the server
    // build: a client that negotiated 2025-11-25 keeps its ping.
    QVERIFY(initializeSession(QtMcp::ProtocolVersion::v2025_11_25));
    QCOMPARE(session()->protocolVersion(), QtMcp::ProtocolVersion::v2025_11_25);

    const auto ping = call<QMcpEmptyResult>(m_client, QMcpPingRequest());
    QVERIFY(ping->answered);
    QVERIFY(!ping->errorCode);
}

void tst_StatelessLifecycle::notificationsNeedAnOptIn()
{
    m_client->setProtocolVersion(QtMcp::ProtocolVersion::v2026_07_28);

    // Opt in to tool list changes only.
    QMcpSubscriptionFilter filter;
    filter.setToolsListChanged(true);
    QMcpSubscriptionsListenRequestParams params;
    params.setNotifications(filter);
    QMcpSubscriptionsListenRequest request;
    request.setParams(params);

    const auto listen = call<QMcpSubscriptionsListenResult>(m_client, request);
    QVERIFY(listen->answered);
    QVERIFY(!listen->errorCode);

    auto *serverSession = session();
    QVERIFY(serverSession);
    QVERIFY(serverSession->hasListenSubscriptions());
    QVERIFY(serverSession->listenSubscriptions().toolsListChanged());
    QVERIFY(!serverSession->listenSubscriptions().promptsListChanged());
    const auto subscriptionId = serverSession->listenSubscriptionId();
    QVERIFY(!subscriptionId.isEmpty());

    // The server has to acknowledge the subscription, reporting the opt-ins it
    // honors and the id every later notification is tagged with.
    QVERIFY(QTest::qWaitFor([this] {
        return !receivedNotification("notifications/subscriptions/acknowledged"_L1).isEmpty();
    }, 5000));
    const auto acknowledged = receivedNotification("notifications/subscriptions/acknowledged"_L1);
    QCOMPARE(subscriptionIdOf(acknowledged), subscriptionId);
    QCOMPARE(acknowledged.value("params"_L1).toObject()
                 .value("notifications"_L1).toObject()
                 .value("toolsListChanged"_L1).toBool(), true);

    // An opted-in change reaches the client, tagged with the subscription id.
    serverSession->registerToolSet(new ToolSet(this));
    QVERIFY(QTest::qWaitFor([this] {
        return !receivedNotification("notifications/tools/list_changed"_L1).isEmpty();
    }, 5000));
    QCOMPARE(subscriptionIdOf(receivedNotification("notifications/tools/list_changed"_L1)),
             subscriptionId);

    // A change the client did not opt in to must not be sent at all.
    QMcpPrompt prompt;
    prompt.setName("test"_L1);
    QMcpPromptMessage message;
    message.setRole(QMcpRole::user);
    message.setContent(QMcpTextContent("Test message"_L1));
    serverSession->appendPrompt(prompt, message);

    QTest::qWait(500);
    QVERIFY(receivedNotification("notifications/prompts/list_changed"_L1).isEmpty());
}

QTEST_MAIN(tst_StatelessLifecycle)
#include "tst_stateless_lifecycle.moc"
