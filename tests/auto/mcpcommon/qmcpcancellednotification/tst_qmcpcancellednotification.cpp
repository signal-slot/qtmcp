// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpCancelledNotification>
#include <QtTest/QTest>

class tst_QMcpCancelledNotification : public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpCancelledNotification::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with numeric request ID and reason
    QTest::newRow("numeric id with reason") << R"({
        "params": {
            "requestId": 123,
            "reason": "User cancelled the operation"
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0"_L1 },
        { "method", "notifications/cancelled"_L1 },
        { "params", QVariantMap {
            { "reason", "User cancelled the operation"_L1 },
            { "requestId", 123 }
        }}
    };

    // Test with string request ID and reason
    QTest::newRow("string id with reason") << R"({
        "params": {
            "requestId": "req-abc-123",
            "reason": "Operation timed out"
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0"_L1 },
        { "method", "notifications/cancelled"_L1 },
        { "params", QVariantMap {
            { "reason", "Operation timed out"_L1 },
            { "requestId", "req-abc-123"_L1 }
        }}
    };

    // Test with numeric request ID without reason
    QTest::newRow("numeric id without reason") << R"({
        "params": {
            "requestId": 456
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0"_L1 },
        { "method", "notifications/cancelled"_L1 },
        { "params", QVariantMap {
            { "requestId", 456 }
        }}
    };

    // Test with string request ID without reason
    QTest::newRow("string id without reason") << R"({
        "params": {
            "requestId": "req-xyz-789"
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0"_L1 },
        { "method", "notifications/cancelled"_L1 },
        { "params", QVariantMap {
            { "requestId", "req-xyz-789"_L1 }
        }}
    };

    // Test with empty reason
    QTest::newRow("empty reason") << R"({
        "params": {
            "requestId": 789,
            "reason": ""
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0"_L1 },
        { "method", "notifications/cancelled"_L1 },
        { "params", QVariantMap {
            { "requestId", 789 }
        }}
    };
}

void tst_QMcpCancelledNotification::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpCancelledNotification notification;
    QVERIFY(notification.fromJsonObject(object));
    TestHelper::verify(&notification, data);

    const auto objectConverted = notification.toJsonObject();
    QCOMPARE(objectConverted, QJsonObject::fromVariantMap(data));
}

void tst_QMcpCancelledNotification::copy_data()
{
    convert_data();
}

void tst_QMcpCancelledNotification::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpCancelledNotification notification;
    QVERIFY(notification.fromJsonObject(object));

    QMcpCancelledNotification notification2(notification);
    TestHelper::verify(&notification2, data);

    QMcpCancelledNotification notification3;
    notification3 = notification2;
    TestHelper::verify(&notification3, data);
}

QTEST_MAIN(tst_QMcpCancelledNotification)
#include "tst_qmcpcancellednotification.moc"
