// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QFuture>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QPromise>
#include <QtCore/QRegularExpression>
#include <QtCore/QTimer>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include <QtMcpClient/QMcpClient>
#include <QtMcpClient/QMcpClientBackendInterface>
#include <QtMcpCommon/QMcpElicitRequestParams>
#include <QtMcpCommon/QMcpEmptyResult>
#include <QtMcpCommon/QMcpInitializeRequest>
#include <QtMcpCommon/QMcpInitializeResult>
#include <QtMcpCommon/QMcpInitializedNotification>
#include <QtMcpCommon/QMcpJSONRPCErrorError>
#include <QtMcpCommon/QMcpRequest>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtMcpServer/QMcpServer>
#include <QtMcpServer/QMcpServerSession>

#include <memory>
#include <optional>

namespace {

// The MRTR plumbing is method agnostic: any handler may answer with an
// input_required interim result. The protocol types ship no request that
// carries inputResponses/requestState in its params, because a retry re-sends
// the *original* request with those two members added. This request models
// exactly that: params are raw JSON, so a test can put inputResponses and
// requestState next to whatever the method itself takes.
class RawParamsRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QJsonObject params READ params WRITE setParams)

public:
    RawParamsRequest() : QMcpRequest(new Private) {}

    QJsonObject params() const { return d<Private>()->params; }

    void setParams(const QJsonObject &params)
    {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject *metaObject() const override { return &staticMetaObject; }

protected:
    struct Private : public QMcpRequest::Private {
        QJsonObject params;

        Private *clone() const override { return new Private(*this); }
    };
};

// Handled synchronously.
class MrtrRequest : public RawParamsRequest
{
    Q_GADGET

public:
    QString method() const final { return "test/mrtr"_L1; }

    const QMetaObject *metaObject() const override { return &staticMetaObject; }
};

// Handled by a QFuture that only finishes later, so that the interim
// substitution has to happen in the future's continuation rather than in the
// synchronous dispatch path.
class MrtrAsyncRequest : public RawParamsRequest
{
    Q_GADGET

public:
    QString method() const final { return "test/mrtr-async"_L1; }

    const QMetaObject *metaObject() const override { return &staticMetaObject; }
};

// What a request callback recorded. Held by shared_ptr so that a late answer,
// arriving after the waiting helper gave up, cannot write to a destroyed stack
// frame.
struct CallState
{
    bool answered = false;
    std::optional<int> errorCode;
};

const auto kRequestState = "opaque-state-token"_L1;
const auto kInputKey = "ask-name"_L1;

QJsonObject nameElicitation()
{
    QMcpElicitRequestParams params;
    params.setMessage("Which name should I use?"_L1);
    QJsonObject requests;
    requests.insert(kInputKey, QMcpServerSession::elicitationInputRequest(params));
    return requests;
}

QJsonObject nameResponses()
{
    QJsonObject content;
    content.insert("name"_L1, "Qt"_L1);
    QJsonObject elicitResult;
    elicitResult.insert("action"_L1, "accept"_L1);
    elicitResult.insert("content"_L1, content);
    QJsonObject responses;
    responses.insert(kInputKey, elicitResult);
    return responses;
}

} // namespace

class tst_Mrtr : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void aHandlerThatNeedsInputAnswersWithAnInterimResult();
    void aRetryCarryingTheResponsesCompletesTheRequest();
    void requireInputIsInertBeforeItsRevision();
    void anAsyncHandlerAnswersTheInterimResultExactlyOnce();

private:
    QMcpServer *m_server = nullptr;
    QMcpClient *m_client = nullptr;
    // Every message the transport handed to the client, so that the tests can
    // count responses and inspect members the typed API drops.
    QList<QJsonObject> m_received;

    // Filled in by the request handlers, so that the tests can assert what the
    // server side observed.
    int m_handlerCalls = 0;
    QJsonObject m_seenInputResponses;
    QJsonValue m_seenRequestState;

    QMcpServerSession *session() const;
    QMcpServerSession *sessionFor(const QUuid &sessionId) const;
    bool initializeSession(QtMcp::ProtocolVersion version);
    int responsesFor(const QJsonValue &id) const;
};

QMcpServerSession *tst_Mrtr::session() const
{
    const auto sessions = m_server->sessions();
    if (sessions.isEmpty())
        return nullptr;
    return sessions.first();
}

QMcpServerSession *tst_Mrtr::sessionFor(const QUuid &sessionId) const
{
    const auto sessions = m_server->sessions();
    for (auto *candidate : sessions) {
        if (candidate->sessionId() == sessionId)
            return candidate;
    }
    return nullptr;
}

int tst_Mrtr::responsesFor(const QJsonValue &id) const
{
    int count = 0;
    for (const auto &object : m_received) {
        if (object.value("id"_L1) == id && (object.contains("result"_L1) || object.contains("error"_L1)))
            ++count;
    }
    return count;
}

void tst_Mrtr::init()
{
    m_received.clear();
    m_handlerCalls = 0;
    m_seenInputResponses = QJsonObject();
    m_seenRequestState = QJsonValue();

    m_server = new QMcpServer("sse"_L1, this);

    // A synchronous handler: it can only finish once the client has answered
    // the elicitation it asks for.
    m_server->addRequestHandler([this](const QUuid &sessionId, const MrtrRequest &,
                                       QMcpJSONRPCErrorError *) -> QMcpEmptyResult {
        ++m_handlerCalls;
        auto *serverSession = sessionFor(sessionId);
        if (serverSession && serverSession->inputResponses().isEmpty()) {
            serverSession->requireInput(nameElicitation(), kRequestState);
            return QMcpEmptyResult();
        }
        if (serverSession) {
            m_seenInputResponses = serverSession->inputResponses();
            m_seenRequestState = serverSession->clientRequestState();
        }
        return QMcpEmptyResult();
    });

    // The same, but the result only becomes available after the event loop has
    // turned a few times.
    m_server->addRequestHandler([this](const QUuid &sessionId, const MrtrAsyncRequest &,
                                       QMcpJSONRPCErrorError *) -> QFuture<QMcpEmptyResult> {
        ++m_handlerCalls;
        if (auto *serverSession = sessionFor(sessionId))
            serverSession->requireInput(nameElicitation(), kRequestState);

        auto promise = std::make_shared<QPromise<QMcpEmptyResult>>();
        promise->start();
        auto future = promise->future();
        QTimer::singleShot(100, this, [promise]() {
            promise->addResult(QMcpEmptyResult());
            promise->finish();
        });
        return future;
    });

    m_server->start("127.0.0.1:10103"_L1);

    m_client = new QMcpClient("sse"_L1, this);

    QSignalSpy startedSpy(m_client, &QMcpClient::started);
    m_client->start("http://localhost:10103"_L1);

    auto *backend = m_client->findChild<QMcpClientBackendInterface *>();
    QVERIFY(backend);
    connect(backend, &QMcpClientBackendInterface::received, this, [this](const QJsonObject &object) {
        m_received.append(object);
    });

    QVERIFY(startedSpy.wait(5000));
    QVERIFY(session());
}

void tst_Mrtr::cleanup()
{
    delete m_client;
    m_client = nullptr;
    delete m_server;
    m_server = nullptr;
}

bool tst_Mrtr::initializeSession(QtMcp::ProtocolVersion version)
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

    m_client->notify(QMcpInitializedNotification());
    return QTest::qWaitFor([this] {
        auto *serverSession = session();
        return serverSession && serverSession->isInitialized();
    }, 5000);
}

void tst_Mrtr::aHandlerThatNeedsInputAnswersWithAnInterimResult()
{
    m_client->setProtocolVersion(QtMcp::ProtocolVersion::v2026_07_28);

    QSignalSpy inputRequiredSpy(m_client, &QMcpClient::inputRequired);

    // The callback of the original request must not run: an interim result
    // does not complete the request, the retry does.
    auto state = std::make_shared<CallState>();
    m_client->request(MrtrRequest(), [state](const QMcpEmptyResult &, const QMcpJSONRPCErrorError *error) {
        state->answered = true;
        if (error)
            state->errorCode = error->code();
    });

    QVERIFY(inputRequiredSpy.wait(5000));
    QCOMPARE(inputRequiredSpy.count(), 1);
    QCOMPARE(m_handlerCalls, 1);
    QVERIFY(!state->answered);

    const auto interim = inputRequiredSpy.at(0).at(1).toJsonObject();
    QCOMPARE(interim.value("resultType"_L1).toString(), "input_required"_L1);

    // The server asks by handing out the very request it would have sent on an
    // older revision, keyed by an identifier the retry has to echo.
    const auto inputRequests = interim.value("inputRequests"_L1).toObject();
    QVERIFY(inputRequests.contains(kInputKey));
    QCOMPARE(inputRequests.value(kInputKey).toObject().value("method"_L1).toString(),
             "elicitation/create"_L1);

    // requestState is what keeps the server stateless across the round trip, so
    // it has to survive verbatim.
    QCOMPARE(interim.value("requestState"_L1).toString(), kRequestState);

    // The interim result names the pending request it belongs to.
    QVERIFY(!inputRequiredSpy.at(0).at(0).toJsonValue().isUndefined());
}

void tst_Mrtr::aRetryCarryingTheResponsesCompletesTheRequest()
{
    m_client->setProtocolVersion(QtMcp::ProtocolVersion::v2026_07_28);

    QSignalSpy inputRequiredSpy(m_client, &QMcpClient::inputRequired);
    m_client->request(MrtrRequest(), [](const QMcpEmptyResult &, const QMcpJSONRPCErrorError *) {});
    QVERIFY(inputRequiredSpy.wait(5000));

    const auto interim = inputRequiredSpy.at(0).at(1).toJsonObject();

    // The retry is the original request again, with the collected answers and
    // the returned requestState added to its params.
    MrtrRequest retry;
    QJsonObject params;
    params.insert("inputResponses"_L1, nameResponses());
    params.insert("requestState"_L1, interim.value("requestState"_L1));
    retry.setParams(params);

    auto state = std::make_shared<CallState>();
    auto result = std::make_shared<std::optional<QMcpEmptyResult>>();
    m_client->request(retry, [state, result](const QMcpEmptyResult &value, const QMcpJSONRPCErrorError *error) {
        state->answered = true;
        if (error)
            state->errorCode = error->code();
        else
            *result = value;
    });

    // This time the request completes through its own callback.
    QVERIFY(QTest::qWaitFor([state] { return state->answered; }, 5000));
    QVERIFY(!state->errorCode);
    QVERIFY(result->has_value());
    QCOMPARE(result->value().resultType(), "complete"_L1);
    QCOMPARE(inputRequiredSpy.count(), 1);
    QCOMPARE(m_handlerCalls, 2);

    // The handler read both members back through the session.
    QCOMPARE(m_seenInputResponses, nameResponses());
    QCOMPARE(m_seenRequestState.toString(), kRequestState);
}

void tst_Mrtr::requireInputIsInertBeforeItsRevision()
{
    // MRTR arrived in 2026-07-28. On an older session the same handler code
    // must not turn its answer into an interim result, otherwise a client that
    // cannot retry would be left waiting forever.
    QVERIFY(initializeSession(QtMcp::ProtocolVersion::v2025_11_25));
    QCOMPARE(session()->protocolVersion(), QtMcp::ProtocolVersion::v2025_11_25);

    QTest::ignoreMessage(QtWarningMsg,
                         QRegularExpression("requireInput\\(\\) needs MCP 2026-07-28"_L1));

    QSignalSpy inputRequiredSpy(m_client, &QMcpClient::inputRequired);

    auto state = std::make_shared<CallState>();
    auto result = std::make_shared<std::optional<QMcpEmptyResult>>();
    m_client->request(MrtrRequest(), [state, result](const QMcpEmptyResult &value, const QMcpJSONRPCErrorError *error) {
        state->answered = true;
        if (error)
            state->errorCode = error->code();
        else
            *result = value;
    });

    QVERIFY(QTest::qWaitFor([state] { return state->answered; }, 5000));
    QVERIFY(!state->errorCode);
    QVERIFY(result->has_value());
    QCOMPARE(inputRequiredSpy.count(), 0);

    // Before 2026-07-28 resultType is not on the wire at all, so the gadget
    // keeps its default.
    QCOMPARE(result->value().resultType(), "complete"_L1);
}

void tst_Mrtr::anAsyncHandlerAnswersTheInterimResultExactlyOnce()
{
    m_client->setProtocolVersion(QtMcp::ProtocolVersion::v2026_07_28);

    QSignalSpy inputRequiredSpy(m_client, &QMcpClient::inputRequired);

    auto state = std::make_shared<CallState>();
    m_client->request(MrtrAsyncRequest(), [state](const QMcpEmptyResult &, const QMcpJSONRPCErrorError *error) {
        state->answered = true;
        if (error)
            state->errorCode = error->code();
    });

    // The handler returns a future that finishes only later; the substitution
    // therefore happens in the future's continuation.
    QVERIFY(inputRequiredSpy.wait(5000));
    QCOMPARE(inputRequiredSpy.count(), 1);

    const auto id = inputRequiredSpy.at(0).at(0).toJsonValue();
    const auto interim = inputRequiredSpy.at(0).at(1).toJsonObject();
    QCOMPARE(interim.value("resultType"_L1).toString(), "input_required"_L1);
    QCOMPARE(interim.value("requestState"_L1).toString(), kRequestState);

    // The two substitution sites (the synchronous dispatch and the future
    // continuation) must not both fire: one request, one response.
    QTest::qWait(500);
    QCOMPARE(responsesFor(id), 1);
    QCOMPARE(inputRequiredSpy.count(), 1);
    QVERIFY(!state->answered);
}

QTEST_MAIN(tst_Mrtr)
#include "tst_mrtr.moc"
