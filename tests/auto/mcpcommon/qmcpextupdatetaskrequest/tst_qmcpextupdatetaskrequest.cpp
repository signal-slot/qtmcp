// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpExtCancelTaskResult>
#include <QtMcpCommon/QMcpExtUpdateTaskRequest>
#include <QtMcpCommon/QMcpExtUpdateTaskResult>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpExtUpdateTaskRequest : public QObject
{
    Q_OBJECT

private slots:
    void methodName();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void rejectsParamsWithoutInputResponses();
    void emptyAcknowledgements();
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
}

void tst_QMcpExtUpdateTaskRequest::methodName()
{
    QMcpExtUpdateTaskRequest request;
    QCOMPARE(request.method(), "tasks/update"_L1);
}

void tst_QMcpExtUpdateTaskRequest::convert_data()
{
    QTest::addColumn<QByteArray>("json");

    // The client answering the elicitation of the specification example.
    QTest::newRow("one response") << R"({
        "jsonrpc": "2.0",
        "id": 6,
        "method": "tasks/update",
        "params": {
            "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
            "inputResponses": {
                "name": {
                    "action": "accept",
                    "content": {
                        "input": "Luca"
                    }
                }
            }
        }
    })"_ba;

    // A server may accept a strict subset of the outstanding keys, so a client
    // may answer several of them at once as well.
    QTest::newRow("two responses") << R"({
        "jsonrpc": "2.0",
        "id": "update-1",
        "method": "tasks/update",
        "params": {
            "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
            "inputResponses": {
                "name": {
                    "action": "accept",
                    "content": {
                        "input": "Luca"
                    }
                },
                "city": {
                    "action": "decline"
                }
            }
        }
    })"_ba;
}

void tst_QMcpExtUpdateTaskRequest::convert()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtUpdateTaskRequest request;
    QVERIFY(request.fromJsonObject(expected));
    QCOMPARE(request.params().taskId(), "786512e2-9e0d-44bd-8f29-789f320fe840"_L1);
    QVERIFY(!request.params().inputResponses().isEmpty());

    // The request has to survive a round trip through JSON unchanged.
    QCOMPARE(request.toJsonObject(), expected);
}

void tst_QMcpExtUpdateTaskRequest::copy_data()
{
    convert_data();
}

void tst_QMcpExtUpdateTaskRequest::copy()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtUpdateTaskRequest request;
    QVERIFY(request.fromJsonObject(expected));

    QMcpExtUpdateTaskRequest copyConstructed(request);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpExtUpdateTaskRequest assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    QMcpExtUpdateTaskRequestParams changed;
    changed.setTaskId("task-changed"_L1);
    changed.setInputResponses(QJsonObject { { "changed"_L1, QJsonObject {} } });
    request.setParams(changed);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpExtUpdateTaskRequest::rejectsParamsWithoutInputResponses()
{
    // Answering nothing is not an update: the schema requires inputResponses
    // next to the task identifier.
    QMcpExtUpdateTaskRequestParams params;
    QVERIFY(!params.fromJsonObject(parse(R"({"taskId": "task-abc123"})"_ba)));
}

void tst_QMcpExtUpdateTaskRequest::emptyAcknowledgements()
{
    // Both tasks/update and tasks/cancel answer with a bare Result. Neither type
    // has members of its own, so they are checked here next to the request they
    // acknowledge rather than in test directories holding a single assertion.
    const QJsonObject ack { { "resultType"_L1, "complete"_L1 } };

    QMcpExtUpdateTaskResult updateResult;
    QCOMPARE(updateResult.resultType(), "complete"_L1);
    QCOMPARE(updateResult.toJsonObject(), ack);
    QVERIFY(updateResult.fromJsonObject(ack));
    QCOMPARE(updateResult.toJsonObject(), ack);

    QMcpExtCancelTaskResult cancelResult;
    QCOMPARE(cancelResult.resultType(), "complete"_L1);
    QCOMPARE(cancelResult.toJsonObject(), ack);
    QVERIFY(cancelResult.fromJsonObject(ack));

    // Unlike the 2025-11-25 core cancel result, the acknowledgement says nothing
    // about the task, not even its identifier.
    QVERIFY(!cancelResult.toJsonObject().contains("taskId"_L1));
    QVERIFY(!cancelResult.toJsonObject().contains("status"_L1));
}

QTEST_MAIN(tst_QMcpExtUpdateTaskRequest)
#include "tst_qmcpextupdatetaskrequest.moc"
