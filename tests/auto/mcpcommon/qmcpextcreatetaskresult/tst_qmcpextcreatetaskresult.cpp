// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpExtCreateTaskResult>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpExtCreateTaskResult : public QObject
{
    Q_OBJECT

private slots:
    void defaultResultType();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void flattenedTask();
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

void tst_QMcpExtCreateTaskResult::defaultResultType()
{
    // The discriminator is what lets a client tell a task handle apart from the
    // standard result of the request, so it must not have to be set by hand.
    QMcpExtCreateTaskResult result;
    QCOMPARE(result.resultType(), "task"_L1);
}

void tst_QMcpExtCreateTaskResult::convert_data()
{
    QTest::addColumn<QByteArray>("json");

    // The seed state of a freshly created task, as in the specification example.
    QTest::newRow("working") << R"({
        "resultType": "task",
        "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
        "status": "working",
        "statusMessage": "The operation is now in progress.",
        "createdAt": "2025-11-25T10:30:00Z",
        "lastUpdatedAt": "2025-11-25T10:40:00Z",
        "ttlMs": 60000,
        "pollIntervalMs": 5000
    })"_ba;

    // A task does not have to start out working, and the TTL may be unlimited.
    QTest::newRow("already failed") << R"({
        "resultType": "task",
        "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
        "status": "failed",
        "statusMessage": "Tool execution failed: API rate limit exceeded",
        "createdAt": "2025-11-25T10:30:00Z",
        "lastUpdatedAt": "2025-11-25T10:40:00Z",
        "ttlMs": null
    })"_ba;
}

void tst_QMcpExtCreateTaskResult::convert()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtCreateTaskResult result;
    QVERIFY(result.fromJsonObject(expected));
    QCOMPARE(result.resultType(), "task"_L1);

    // The result has to survive a round trip through JSON unchanged.
    QCOMPARE(result.toJsonObject(), expected);
}

void tst_QMcpExtCreateTaskResult::copy_data()
{
    convert_data();
}

void tst_QMcpExtCreateTaskResult::copy()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtCreateTaskResult result;
    QVERIFY(result.fromJsonObject(expected));

    QMcpExtCreateTaskResult copyConstructed(result);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpExtCreateTaskResult assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    result.setTaskId("task-changed"_L1);
    result.setStatus(QMcpTaskStatus::cancelled);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpExtCreateTaskResult::flattenedTask()
{
    // Unlike the 2025-11-25 core result, which nests the task under a "task"
    // member, the extension defines the result as Result & Task and therefore
    // puts the task members next to resultType.
    QMcpExtCreateTaskResult result;
    result.setTaskId("786512e2-9e0d-44bd-8f29-789f320fe840"_L1);
    result.setCreatedAt("2025-11-25T10:30:00Z"_L1);
    result.setLastUpdatedAt("2025-11-25T10:40:00Z"_L1);
    result.setTtlMs(QJsonValue(60000));

    const auto object = result.toJsonObject();
    QVERIFY(!object.contains("task"_L1));
    QCOMPARE(object.value("resultType"_L1).toString(), "task"_L1);
    QCOMPARE(object.value("taskId"_L1).toString(), "786512e2-9e0d-44bd-8f29-789f320fe840"_L1);
    QCOMPARE(object.value("status"_L1).toString(), "working"_L1);
    QCOMPARE(object.value("ttlMs"_L1).toInteger(), 60000);

    // resultType only exists from 2026-07-28 on, and "task" is the whole point
    // of this type, so an older revision cannot carry the discriminator at all.
    // Pin the boundary here: it is what keeps the extension from being spoken to
    // a client that predates the field.
    const auto oldObject = result.toJsonObject(QtMcp::ProtocolVersion::v2025_11_25);
    QVERIFY(!oldObject.contains("resultType"_L1));
}

QTEST_MAIN(tst_QMcpExtCreateTaskResult)
#include "tst_qmcpextcreatetaskresult.moc"
