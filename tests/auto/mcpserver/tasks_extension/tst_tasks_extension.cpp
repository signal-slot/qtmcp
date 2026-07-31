// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QFuture>
#include <QtCore/QJsonObject>
#include <QtCore/QPromise>
#include <QtCore/QTimer>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include <QtMcpClient/QMcpClient>
#include <QtMcpClient/QMcpClientBackendInterface>
#include <QtMcpCommon/QMcpCallToolRequest>
#include <QtMcpCommon/QMcpCallToolResult>
#include <QtMcpCommon/QMcpCallToolResultContent>
#include <QtMcpCommon/QMcpCancelTaskRequest>
#include <QtMcpCommon/QMcpDiscoverRequest>
#include <QtMcpCommon/QMcpDiscoverResult>
#include <QtMcpCommon/QMcpExtCancelTaskResult>
#include <QtMcpCommon/QMcpExtCreateTaskResult>
#include <QtMcpCommon/QMcpExtGetTaskResult>
#include <QtMcpCommon/QMcpGetTaskRequest>
#include <QtMcpCommon/QMcpJSONRPCErrorError>
#include <QtMcpCommon/QMcpTextContent>
#include <QtMcpCommon/qmcptaskstatus.h>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtMcpServer/QMcpServer>
#include <QtMcpServer/QMcpServerSession>

#include <memory>
#include <optional>

namespace {

const auto kTasksExtension = "io.modelcontextprotocol/tasks"_L1;
const auto kToolName = "slowEcho"_L1;

// A tool whose future is deliberately still unfinished when the handler
// returns: that is the precondition for the server to hand out a task instead
// of answering the tools/call directly.
class SlowToolSet : public QObject
{
    Q_OBJECT
public:
    explicit SlowToolSet(int delayMs, QObject *parent = nullptr)
        : QObject(parent), m_delayMs(delayMs) {}

    Q_INVOKABLE QFuture<QList<QMcpCallToolResultContent>> slowEcho(const QString &message)
    {
        auto promise = std::make_shared<QPromise<QList<QMcpCallToolResultContent>>>();
        promise->start();
        auto future = promise->future();
        QTimer::singleShot(m_delayMs, this, [promise, message]() {
            promise->addResult({ QMcpTextContent(message) });
            promise->finish();
        });
        return future;
    }

private:
    int m_delayMs;
};

// What a request callback recorded. Held by shared_ptr so that a late answer,
// arriving after the waiting helper gave up, cannot write to a destroyed stack
// frame.
template<typename Result>
struct CallState
{
    bool answered = false;
    std::optional<int> errorCode;
    Result result;
};

template<typename Result, typename Request>
std::shared_ptr<CallState<Result>> call(QMcpClient *client, const Request &request, int timeout = 5000)
{
    auto state = std::make_shared<CallState<Result>>();
    client->request(request, [state](const Result &result, const QMcpJSONRPCErrorError *error) {
        state->answered = true;
        if (error)
            state->errorCode = error->code();
        else
            state->result = result;
    });
    QTest::qWaitFor([state] { return state->answered; }, timeout);
    return state;
}

QMcpCallToolRequest slowEchoRequest(const QString &message)
{
    QMcpCallToolRequest request;
    auto params = request.params();
    params.setName(kToolName);
    QJsonObject arguments;
    arguments.insert("message"_L1, message);
    params.setArguments(arguments);
    request.setParams(params);
    return request;
}

QMcpGetTaskRequest getTaskRequest(const QString &taskId)
{
    QMcpGetTaskRequest request;
    auto params = request.params();
    params.setTaskId(taskId);
    request.setParams(params);
    return request;
}

} // namespace

class tst_TasksExtension : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void aSlowToolCallHandsOutATaskHandle();
    void pollingReportsWorkingUntilTheToolFinishes();
    void cancellingATaskIsAcknowledgedAndStopsTheWork();
    void withoutTheClientOptInTheCallStaysSynchronous();
    void discoverAdvertisesTheExtension();
    void anUnknownTaskIdIsRejected();

private:
    QMcpServer *m_server = nullptr;
    QMcpClient *m_client = nullptr;
    // The raw messages the transport handed to the client: the wire shape of a
    // CreateTaskResult is what a foreign client sees, so the tests check it
    // directly rather than only through the gadgets.
    QList<QJsonObject> m_received;

    void startServer(int toolDelayMs);
    void startClient(bool tasksExtensionEnabled);
    QJsonObject receivedResultWithType(const QString &resultType) const;
};

void tst_TasksExtension::startServer(int toolDelayMs)
{
    m_server = new QMcpServer("sse"_L1, this);
    m_server->setTasksExtensionEnabled(true);
    m_server->registerToolSet(new SlowToolSet(toolDelayMs, m_server),
                              { { kToolName, "Echoes its argument, slowly"_L1 } });
    m_server->start("127.0.0.1:10104"_L1);
}

void tst_TasksExtension::startClient(bool tasksExtensionEnabled)
{
    m_client = new QMcpClient("sse"_L1, this);
    m_client->setProtocolVersion(QtMcp::ProtocolVersion::v2026_07_28);
    m_client->setTasksExtensionEnabled(tasksExtensionEnabled);

    QSignalSpy startedSpy(m_client, &QMcpClient::started);
    m_client->start("http://localhost:10104"_L1);

    auto *backend = m_client->findChild<QMcpClientBackendInterface *>();
    QVERIFY(backend);
    connect(backend, &QMcpClientBackendInterface::received, this, [this](const QJsonObject &object) {
        m_received.append(object);
    });

    QVERIFY(startedSpy.wait(5000));
    QVERIFY(!m_server->sessions().isEmpty());
}

QJsonObject tst_TasksExtension::receivedResultWithType(const QString &resultType) const
{
    for (const auto &object : m_received) {
        const auto result = object.value("result"_L1).toObject();
        if (result.value("resultType"_L1).toString() == resultType)
            return result;
    }
    return {};
}

void tst_TasksExtension::init()
{
    m_received.clear();
}

void tst_TasksExtension::cleanup()
{
    delete m_client;
    m_client = nullptr;
    delete m_server;
    m_server = nullptr;
}

void tst_TasksExtension::aSlowToolCallHandsOutATaskHandle()
{
    startServer(200);
    startClient(true);

    const auto created = call<QMcpExtCreateTaskResult>(m_client, slowEchoRequest("hello"_L1));
    QVERIFY(created->answered);
    QVERIFY(!created->errorCode);

    // The wire shape decides whether a foreign client can use the handle:
    // CreateTaskResult is "Result & Task", so the task members sit next to
    // resultType rather than inside a nested object.
    const auto raw = receivedResultWithType("task"_L1);
    QVERIFY(!raw.isEmpty());
    QVERIFY(!raw.value("taskId"_L1).toString().isEmpty());
    QCOMPARE(raw.value("status"_L1).toString(), "working"_L1);
    QVERIFY(!raw.value("createdAt"_L1).toString().isEmpty());
    QVERIFY(!raw.value("lastUpdatedAt"_L1).toString().isEmpty());
    QVERIFY(raw.contains("ttlMs"_L1));
    QVERIFY(raw.contains("pollIntervalMs"_L1));

    // The same, read back through the type the library ships for it.
    QCOMPARE(created->result.resultType(), "task"_L1);
    QVERIFY(!created->result.taskId().isEmpty());
    QCOMPARE(created->result.status(), QMcpTaskStatus::working);
    QCOMPARE(created->result.taskId(), raw.value("taskId"_L1).toString());
}

void tst_TasksExtension::pollingReportsWorkingUntilTheToolFinishes()
{
    startServer(400);
    startClient(true);

    const auto created = call<QMcpExtCreateTaskResult>(m_client, slowEchoRequest("hello"_L1));
    QVERIFY(created->answered);
    const auto taskId = created->result.taskId();
    QVERIFY(!taskId.isEmpty());

    // The tool is still running, so the task is not terminal yet.
    const auto working = call<QMcpExtGetTaskResult>(m_client, getTaskRequest(taskId));
    QVERIFY(working->answered);
    QVERIFY(!working->errorCode);
    QCOMPARE(working->result.resultType(), "complete"_L1);
    QCOMPARE(working->result.taskId(), taskId);
    QCOMPARE(working->result.status(), QMcpTaskStatus::working);
    QVERIFY(working->result.result().isEmpty());

    QTest::qWait(600);

    // Once the future finished, the very same poll returns the tool's own
    // CallToolResult as the task result.
    const auto completed = call<QMcpExtGetTaskResult>(m_client, getTaskRequest(taskId));
    QVERIFY(completed->answered);
    QVERIFY(!completed->errorCode);
    QCOMPARE(completed->result.status(), QMcpTaskStatus::completed);

    QMcpCallToolResult toolResult;
    QVERIFY(toolResult.fromJsonObject(completed->result.result(), QtMcp::ProtocolVersion::v2026_07_28));
    QCOMPARE(toolResult.content().size(), 1);
    QCOMPARE(toolResult.content().first().textContent().text(), "hello"_L1);
}

void tst_TasksExtension::cancellingATaskIsAcknowledgedAndStopsTheWork()
{
    startServer(400);
    startClient(true);

    const auto created = call<QMcpExtCreateTaskResult>(m_client, slowEchoRequest("hello"_L1));
    QVERIFY(created->answered);
    const auto taskId = created->result.taskId();
    QVERIFY(!taskId.isEmpty());

    QMcpCancelTaskRequest cancel;
    auto cancelParams = cancel.params();
    cancelParams.setTaskId(taskId);
    cancel.setParams(cancelParams);

    // Cancellation is cooperative: the server owes an acknowledgement, and the
    // acknowledgement is an ordinary result, not a task handle.
    const auto cancelled = call<QMcpExtCancelTaskResult>(m_client, cancel);
    QVERIFY(cancelled->answered);
    QVERIFY(!cancelled->errorCode);
    QCOMPARE(cancelled->result.resultType(), "complete"_L1);

    // What "cooperative" means here in Qt terms: the server cancels the future
    // it holds, which is the .then() continuation QMcpServerSession::
    // callToolAsync() built on top of the tool's own future. QFuture::cancel()
    // does not travel back up such a chain - the tool's QPromise never reports
    // isCanceled() and keeps running to completion. What the cancellation does
    // reach is the server's own continuation: it is skipped, its onCanceled
    // handler records the terminal status, and the tool's late result is
    // dropped. So the task does end up cancelled, and it stays cancelled even
    // after the tool would have finished (the wait below outlives the tool's
    // 400 ms). A server that wanted to stop the work itself would need to
    // forward the cancellation to the tool, which the extension does not
    // require: the acknowledgement above is all a server must guarantee.
    QTest::qWait(600);
    const auto polled = call<QMcpExtGetTaskResult>(m_client, getTaskRequest(taskId));
    QVERIFY(polled->answered);
    QVERIFY(!polled->errorCode);
    QCOMPARE(polled->result.status(), QMcpTaskStatus::cancelled);
    QVERIFY(polled->result.result().isEmpty());
}

void tst_TasksExtension::withoutTheClientOptInTheCallStaysSynchronous()
{
    startServer(200);
    startClient(false);

    // Task creation is opt-in per request: a client that does not declare the
    // extension must keep getting the final result, however long it takes.
    const auto called = call<QMcpCallToolResult>(m_client, slowEchoRequest("hello"_L1));
    QVERIFY(called->answered);
    QVERIFY(!called->errorCode);
    QCOMPARE(called->result.resultType(), "complete"_L1);
    QCOMPARE(called->result.content().size(), 1);
    QCOMPARE(called->result.content().first().textContent().text(), "hello"_L1);

    QVERIFY(receivedResultWithType("task"_L1).isEmpty());
}

void tst_TasksExtension::discoverAdvertisesTheExtension()
{
    startServer(200);
    startClient(true);

    // server/discover is how a client learns the server speaks the extension
    // before it declares the extension itself.
    const auto discovered = call<QMcpDiscoverResult>(m_client, QMcpDiscoverRequest());
    QVERIFY(discovered->answered);
    QVERIFY(!discovered->errorCode);
    QVERIFY(discovered->result.capabilities().extensions().contains(kTasksExtension));
}

void tst_TasksExtension::anUnknownTaskIdIsRejected()
{
    startServer(200);
    startClient(true);

    // Polling a task the server never handed out is a client mistake, not an
    // empty task: it has to be an invalid params error.
    const auto polled = call<QMcpExtGetTaskResult>(m_client, getTaskRequest("no-such-task"_L1));
    QVERIFY(polled->answered);
    QCOMPARE(polled->errorCode, -32602);
}

QTEST_MAIN(tst_TasksExtension)
#include "tst_tasks_extension.moc"
