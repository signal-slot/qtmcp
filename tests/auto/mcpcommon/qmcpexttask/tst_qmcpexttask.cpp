// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpExtTask>
#include <QtMcpCommon/QMcpTask>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpExtTask : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void millisecondSuffixedKeys();
    void rejectsIncompleteObject();
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

void tst_QMcpExtTask::convert_data()
{
    QTest::addColumn<QByteArray>("json");

    // Every member the extension defines, with a numeric TTL.
    QTest::newRow("working") << R"({
        "taskId": "task-abc123",
        "status": "working",
        "statusMessage": "The operation is now in progress.",
        "createdAt": "2025-11-25T09:30:00Z",
        "lastUpdatedAt": "2025-11-25T09:31:00Z",
        "ttlMs": 3600000,
        "pollIntervalMs": 500
    })"_ba;

    // ttlMs is required but nullable, and null means unlimited retention, so a
    // task may legitimately carry no TTL at all.
    QTest::newRow("unlimited ttl") << R"({
        "taskId": "task-abc123",
        "status": "input_required",
        "createdAt": "2025-11-25T09:30:00Z",
        "lastUpdatedAt": "2025-11-25T09:31:00Z",
        "ttlMs": null
    })"_ba;

    // The only members that must be there.
    QTest::newRow("required only") << R"({
        "taskId": "task-abc123",
        "status": "completed",
        "createdAt": "2025-11-25T09:30:00Z",
        "lastUpdatedAt": "2025-11-25T09:31:00Z",
        "ttlMs": 3600000
    })"_ba;
}

void tst_QMcpExtTask::convert()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtTask task;
    QVERIFY(task.fromJsonObject(expected));

    // The task has to survive a round trip through JSON unchanged.
    QCOMPARE(task.toJsonObject(), expected);
}

void tst_QMcpExtTask::copy_data()
{
    convert_data();
}

void tst_QMcpExtTask::copy()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtTask task;
    QVERIFY(task.fromJsonObject(expected));

    QMcpExtTask copyConstructed(task);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpExtTask assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    task.setTaskId("task-changed"_L1);
    task.setTtlMs(QJsonValue(1));
    task.setPollIntervalMs(999);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpExtTask::millisecondSuffixedKeys()
{
    // This is the reason the extension task is a type of its own rather than a
    // few extra members on QMcpTask: the extension renamed the two duration
    // members, and QMcpGadget takes the JSON key from the property name.
    QMcpExtTask task;
    task.setTaskId("task-abc123"_L1);
    task.setCreatedAt("2025-11-25T09:30:00Z"_L1);
    task.setLastUpdatedAt("2025-11-25T09:31:00Z"_L1);
    task.setTtlMs(QJsonValue(3600000));
    task.setPollIntervalMs(500);

    const auto object = task.toJsonObject();
    QCOMPARE(object.value("ttlMs"_L1).toInteger(), 3600000);
    QCOMPARE(object.value("pollIntervalMs"_L1).toInt(), 500);
    QVERIFY(!object.contains("ttl"_L1));
    QVERIFY(!object.contains("pollInterval"_L1));

    // The core 2025-11-25 task spells them the other way around, so it cannot
    // read what the extension writes: its required ttl is missing here.
    QMcpTask core;
    QVERIFY(!core.fromJsonObject(object, QtMcp::ProtocolVersion::v2025_11_25));
}

void tst_QMcpExtTask::rejectsIncompleteObject()
{
    // taskId alone leaves four required members unset, so the object is not a
    // task and must not be accepted as one.
    QMcpExtTask task;
    QVERIFY(!task.fromJsonObject(parse(R"({"taskId": "task-abc123"})"_ba)));
}

QTEST_MAIN(tst_QMcpExtTask)
#include "tst_qmcpexttask.moc"
