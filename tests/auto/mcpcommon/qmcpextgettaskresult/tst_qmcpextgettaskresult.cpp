// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpExtGetTaskResult>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpExtGetTaskResult : public QObject
{
    Q_OBJECT

private slots:
    void defaultResultType();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void completedCarriesResult();
    void failedCarriesError();
    void inputRequiredCarriesInputRequests();
    void payloadsAreOmittedWhenUnset();
};

namespace {
QJsonObject parse(const QByteArray &json)
{
    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError)
        qFatal("bad test json: %s", qPrintable(error.errorString()));
    return doc.object();
}

// The per-status bodies are needed both as test data and as the input of the
// dedicated payload checks, so they are named here.
constexpr auto workingJson = R"({
    "resultType": "complete",
    "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
    "status": "working",
    "createdAt": "2025-11-25T10:30:00Z",
    "lastUpdatedAt": "2025-11-25T10:50:00Z",
    "ttlMs": 3600000,
    "pollIntervalMs": 5000
})";

constexpr auto inputRequiredJson = R"({
    "resultType": "complete",
    "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
    "status": "input_required",
    "createdAt": "2025-11-25T10:30:00Z",
    "lastUpdatedAt": "2025-11-25T10:50:00Z",
    "ttlMs": 3600000,
    "pollIntervalMs": 5000,
    "inputRequests": {
        "name": {
            "method": "elicitation/create",
            "params": {
                "mode": "form",
                "message": "Please enter your name.",
                "requestedSchema": {
                    "type": "object",
                    "properties": {
                        "name": { "type": "string" }
                    },
                    "required": ["name"]
                }
            }
        }
    }
})";

constexpr auto completedJson = R"({
    "resultType": "complete",
    "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
    "status": "completed",
    "createdAt": "2025-11-25T10:30:00Z",
    "lastUpdatedAt": "2025-11-25T10:50:00Z",
    "ttlMs": 3600000,
    "pollIntervalMs": 5000,
    "result": {
        "content": [
            {
                "type": "text",
                "text": "Hello, Luca!"
            }
        ],
        "isError": false
    }
})";

constexpr auto failedJson = R"({
    "resultType": "complete",
    "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
    "status": "failed",
    "statusMessage": "Tool execution failed: API rate limit exceeded",
    "createdAt": "2025-11-25T10:30:00Z",
    "lastUpdatedAt": "2025-11-25T10:40:00Z",
    "ttlMs": 3600000,
    "error": {
        "code": -32603,
        "message": "API rate limit exceeded"
    }
})";

constexpr auto cancelledJson = R"({
    "resultType": "complete",
    "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
    "status": "cancelled",
    "statusMessage": "cancelled by user",
    "createdAt": "2025-11-25T10:30:00Z",
    "lastUpdatedAt": "2025-11-25T10:50:00Z",
    "ttlMs": null
})";
}

void tst_QMcpExtGetTaskResult::defaultResultType()
{
    // tasks/get returns the standard result shape of its own request, so the
    // discriminator says "complete" and never "task".
    QMcpExtGetTaskResult result;
    QCOMPARE(result.resultType(), "complete"_L1);
}

void tst_QMcpExtGetTaskResult::convert_data()
{
    QTest::addColumn<QByteArray>("json");

    // One row per DetailedTask variant.
    QTest::newRow("working") << QByteArray(workingJson);
    QTest::newRow("input_required") << QByteArray(inputRequiredJson);
    QTest::newRow("completed") << QByteArray(completedJson);
    QTest::newRow("failed") << QByteArray(failedJson);
    QTest::newRow("cancelled") << QByteArray(cancelledJson);
}

void tst_QMcpExtGetTaskResult::convert()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtGetTaskResult result;
    QVERIFY(result.fromJsonObject(expected));

    // The result has to survive a round trip through JSON unchanged, payload
    // and all.
    QCOMPARE(result.toJsonObject(), expected);
}

void tst_QMcpExtGetTaskResult::copy_data()
{
    convert_data();
}

void tst_QMcpExtGetTaskResult::copy()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtGetTaskResult result;
    QVERIFY(result.fromJsonObject(expected));

    QMcpExtGetTaskResult copyConstructed(result);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpExtGetTaskResult assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    result.setTaskId("task-changed"_L1);
    result.setResult(QJsonObject { { "changed"_L1, true } });
    result.setError(QJsonObject { { "changed"_L1, true } });
    result.setInputRequests(QJsonObject { { "changed"_L1, QJsonObject {} } });
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpExtGetTaskResult::completedCarriesResult()
{
    QMcpExtGetTaskResult result;
    QVERIFY(result.fromJsonObject(parse(QByteArray(completedJson))));

    QCOMPARE(result.status(), QMcpTaskStatus::completed);

    // The final result is whatever the augmented request would have returned;
    // here a CallToolResult.
    const auto payload = result.result();
    QCOMPARE(payload.value("isError"_L1).toBool(), false);
    const auto content = payload.value("content"_L1).toArray();
    QCOMPARE(content.size(), 1);
    QCOMPARE(content.at(0).toObject().value("text"_L1).toString(), "Hello, Luca!"_L1);

    // A completed task reports no protocol error.
    QVERIFY(result.error().isEmpty());
    QVERIFY(result.inputRequests().isEmpty());
}

void tst_QMcpExtGetTaskResult::failedCarriesError()
{
    QMcpExtGetTaskResult result;
    QVERIFY(result.fromJsonObject(parse(QByteArray(failedJson))));

    QCOMPARE(result.status(), QMcpTaskStatus::failed);

    // "failed" is reserved for JSON-RPC errors during execution, so the error
    // member holds the JSON-RPC error object itself.
    const auto error = result.error();
    QCOMPARE(error.value("code"_L1).toInt(), -32603);
    QCOMPARE(error.value("message"_L1).toString(), "API rate limit exceeded"_L1);
    QCOMPARE(result.statusMessage(), "Tool execution failed: API rate limit exceeded"_L1);

    // A failed task has no result to report.
    QVERIFY(result.result().isEmpty());
}

void tst_QMcpExtGetTaskResult::inputRequiredCarriesInputRequests()
{
    QMcpExtGetTaskResult result;
    QVERIFY(result.fromJsonObject(parse(QByteArray(inputRequiredJson))));

    QCOMPARE(result.status(), QMcpTaskStatus::input_required);

    // The keys are server chosen and match the ones the client answers with in
    // a tasks/update request.
    const auto requests = result.inputRequests();
    QCOMPARE(requests.keys(), QStringList { "name"_L1 });
    QCOMPARE(requests.value("name"_L1).toObject().value("method"_L1).toString(),
             "elicitation/create"_L1);

    QVERIFY(result.result().isEmpty());
    QVERIFY(result.error().isEmpty());
}

void tst_QMcpExtGetTaskResult::payloadsAreOmittedWhenUnset()
{
    // The three payloads are mutually exclusive, so a working task must not
    // carry empty objects for the two it has no use for.
    QMcpExtGetTaskResult result;
    QVERIFY(result.fromJsonObject(parse(QByteArray(workingJson))));

    const auto object = result.toJsonObject();
    QVERIFY(!object.contains("result"_L1));
    QVERIFY(!object.contains("error"_L1));
    QVERIFY(!object.contains("inputRequests"_L1));
}

QTEST_MAIN(tst_QMcpExtGetTaskResult)
#include "tst_qmcpextgettaskresult.moc"
