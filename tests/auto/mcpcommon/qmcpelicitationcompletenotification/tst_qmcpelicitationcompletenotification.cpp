// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpElicitationCompleteNotification>
#include <QtTest/QTest>

class tst_QMcpElicitationCompleteNotification : public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void method();
    void missingElicitationId();
};

void tst_QMcpElicitationCompleteNotification::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    QTest::newRow("elicitationId") << R"({
        "params": {
            "elicitationId": "elicit-1"
        }
    })"_ba
    << QVariantMap {
        { "jsonrpc", "2.0"_L1 },
        { "method", "notifications/elicitation/complete"_L1 },
        { "params", QVariantMap {
            { "elicitationId", "elicit-1"_L1 }
        }}
    };
}

void tst_QMcpElicitationCompleteNotification::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpElicitationCompleteNotification notification;
    QVERIFY(notification.fromJsonObject(doc.object()));
    TestHelper::verify(&notification, data);

    QCOMPARE(notification.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpElicitationCompleteNotification::copy_data()
{
    convert_data();
}

void tst_QMcpElicitationCompleteNotification::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpElicitationCompleteNotification notification;
    QVERIFY(notification.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpElicitationCompleteNotification copyConstructed(notification);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpElicitationCompleteNotification assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    auto params = notification.params();
    params.setElicitationId("changed"_L1);
    notification.setParams(params);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpElicitationCompleteNotification::method()
{
    // The method is constant and part of every serialization.
    const QMcpElicitationCompleteNotification notification;
    QCOMPARE(notification.method(), "notifications/elicitation/complete"_L1);
}

void tst_QMcpElicitationCompleteNotification::missingElicitationId()
{
    // elicitationId is the only member and it is required.
    QMcpElicitationCompleteNotification notification;
    QVERIFY(!notification.fromJsonObject(QJsonObject { { "params"_L1, QJsonObject {} } }));
}

QTEST_MAIN(tst_QMcpElicitationCompleteNotification)
#include "tst_qmcpelicitationcompletenotification.moc"
