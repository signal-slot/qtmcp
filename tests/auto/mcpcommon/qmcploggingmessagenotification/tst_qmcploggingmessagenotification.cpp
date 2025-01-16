// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpLoggingMessageNotification>
#include <QtMcpCommon/QMcpLoggingMessageNotificationParams>
#include <QtTest/QTest>

class tst_QMcpLoggingMessageNotification : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpLoggingMessageNotification::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with string data
    QTest::newRow("string data") << R"({
        "params": {
            "data": "Test message",
            "level": "warning",
            "logger": "TestLogger"
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0" },
        { "method", "notifications/message" },
        { "params", QVariantMap {
            { "data", "Test message" },
            { "level", QVariant::fromValue(QMcpLoggingLevel::warning) },
            { "logger", "TestLogger" }
        }}
    };

    // Test with object data
    QTest::newRow("object data") << R"({
        "params": {
            "data": {
                "message": "Test message",
                "code": 123,
                "details": ["detail1", "detail2"]
            },
            "level": "error",
            "logger": "SystemLogger"
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0" },
        { "method", "notifications/message" },
        { "params", QVariantMap {
            { "data", QVariantMap {
                { "message", "Test message" },
                { "code", 123 },
                { "details", QVariantList { "detail1", "detail2" } }
            }},
            { "level", QVariant::fromValue(QMcpLoggingLevel::error) },
            { "logger", "SystemLogger" }
        }}
    };

    // Test without optional logger
    QTest::newRow("no logger") << R"({
        "params": {
            "data": "Simple message",
            "level": "info"
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0" },
        { "method", "notifications/message" },
        { "params", QVariantMap {
            { "data", "Simple message" },
            { "level", QVariant::fromValue(QMcpLoggingLevel::info) },
        }}
    };
}

void tst_QMcpLoggingMessageNotification::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpLoggingMessageNotification notification;
    QVERIFY(notification.fromJsonObject(object));
    TestHelper::verify(&notification, data);

    // Verify conversion
    const auto converted = notification.toJsonObject();
    QCOMPARE(converted, QJsonObject::fromVariantMap(data));
}

void tst_QMcpLoggingMessageNotification::copy_data()
{
    convert_data();
}

void tst_QMcpLoggingMessageNotification::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpLoggingMessageNotification notification;
    QVERIFY(notification.fromJsonObject(object));

    // Test copy constructor
    QMcpLoggingMessageNotification notification2(notification);
    QCOMPARE(notification2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpLoggingMessageNotification notification3;
    notification3 = notification2;
    QCOMPARE(notification3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpLoggingMessageNotification)
#include "tst_qmcploggingmessagenotification.moc"
