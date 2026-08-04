// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpExtTaskStatusNotification>
#include <QtMcpCommon/QMcpTaskStatusNotification>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpExtTaskStatusNotification : public QObject
{
    Q_OBJECT

private slots:
    void methodName();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void carriesTheCompleteTask();
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

constexpr auto completedJson = R"({
    "jsonrpc": "2.0",
    "method": "notifications/tasks",
    "params": {
        "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
        "status": "completed",
        "createdAt": "2025-11-25T10:30:00Z",
        "lastUpdatedAt": "2025-11-25T10:50:00Z",
        "ttlMs": 60000,
        "pollIntervalMs": 5000,
        "result": {
            "content": [
                {
                    "type": "text",
                    "text": "Operation completed successfully."
                }
            ],
            "isError": false
        }
    }
})";
}

void tst_QMcpExtTaskStatusNotification::methodName()
{
    // The extension dropped the /status suffix the 2025-11-25 core notification
    // still carries. Both spellings exist side by side, one type each.
    QMcpExtTaskStatusNotification notification;
    QCOMPARE(notification.method(), "notifications/tasks"_L1);

    QMcpTaskStatusNotification core;
    QCOMPARE(core.method(), "notifications/tasks/status"_L1);
}

void tst_QMcpExtTaskStatusNotification::convert_data()
{
    QTest::addColumn<QByteArray>("json");

    // A notification carries the complete task, so it repeats every shape
    // tasks/get can return.
    QTest::newRow("completed") << QByteArray(completedJson);

    QTest::newRow("working") << R"({
        "jsonrpc": "2.0",
        "method": "notifications/tasks",
        "params": {
            "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
            "status": "working",
            "statusMessage": "still going",
            "createdAt": "2025-11-25T10:30:00Z",
            "lastUpdatedAt": "2025-11-25T10:50:00Z",
            "ttlMs": null
        }
    })"_ba;

    QTest::newRow("failed") << R"({
        "jsonrpc": "2.0",
        "method": "notifications/tasks",
        "params": {
            "taskId": "786512e2-9e0d-44bd-8f29-789f320fe840",
            "status": "failed",
            "createdAt": "2025-11-25T10:30:00Z",
            "lastUpdatedAt": "2025-11-25T10:40:00Z",
            "ttlMs": 60000,
            "error": {
                "code": -32603,
                "message": "API rate limit exceeded"
            }
        }
    })"_ba;
}

void tst_QMcpExtTaskStatusNotification::convert()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtTaskStatusNotification notification;
    QVERIFY(notification.fromJsonObject(expected));

    // The notification has to survive a round trip through JSON unchanged.
    QCOMPARE(notification.toJsonObject(), expected);
}

void tst_QMcpExtTaskStatusNotification::copy_data()
{
    convert_data();
}

void tst_QMcpExtTaskStatusNotification::copy()
{
    QFETCH(QByteArray, json);

    const auto expected = parse(json);

    QMcpExtTaskStatusNotification notification;
    QVERIFY(notification.fromJsonObject(expected));

    QMcpExtTaskStatusNotification copyConstructed(notification);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpExtTaskStatusNotification assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    QMcpExtTaskStatusNotificationParams changed;
    changed.setTaskId("task-changed"_L1);
    notification.setParams(changed);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpExtTaskStatusNotification::carriesTheCompleteTask()
{
    // The point of subscribing is not having to poll, so the notification has to
    // deliver the final result too, not just the status.
    QMcpExtTaskStatusNotification notification;
    QVERIFY(notification.fromJsonObject(parse(QByteArray(completedJson))));

    const auto params = notification.params();
    QCOMPARE(params.status(), QMcpTaskStatus::completed);
    QCOMPARE(params.taskId(), "786512e2-9e0d-44bd-8f29-789f320fe840"_L1);
    QCOMPARE(params.pollIntervalMs(), 5000);
    QCOMPARE(params.ttlMs().toInteger(), 60000);
    QCOMPARE(params.result().value("isError"_L1).toBool(), false);
    QVERIFY(params.error().isEmpty());
}

QTEST_MAIN(tst_QMcpExtTaskStatusNotification)
#include "tst_qmcpexttaskstatusnotification.moc"
