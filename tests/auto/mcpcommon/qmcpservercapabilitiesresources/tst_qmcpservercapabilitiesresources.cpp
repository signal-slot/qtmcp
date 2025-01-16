// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpServerCapabilitiesResources>
#include <QtTest/QTest>

class tst_QMcpServerCapabilitiesResources : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpServerCapabilitiesResources::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Default values (both false)
    QTest::newRow("default") << R"({
    })"_ba
    << QVariantMap {
    };

    // Only listChanged enabled
    QTest::newRow("listChanged only") << R"({
        "listChanged": true
    })"_ba
    << QVariantMap {
        { "listChanged", true },
    };

    // Only subscribe enabled
    QTest::newRow("subscribe only") << R"({
        "subscribe": true
    })"_ba
    << QVariantMap {
        { "subscribe", true }
    };

    // Both enabled
    QTest::newRow("both enabled") << R"({
        "listChanged": true,
        "subscribe": true
    })"_ba
    << QVariantMap {
        { "listChanged", true },
        { "subscribe", true }
    };
}

void tst_QMcpServerCapabilitiesResources::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpServerCapabilitiesResources capabilities;
    QVERIFY(capabilities.fromJsonObject(object));
    TestHelper::verify(&capabilities, data);

    // Verify conversion
    const auto converted = capabilities.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);

    // Verify the property values match
    QCOMPARE(capabilities.listChanged(), data["listChanged"].toBool());
    QCOMPARE(capabilities.subscribe(), data["subscribe"].toBool());
}

void tst_QMcpServerCapabilitiesResources::copy_data()
{
    convert_data();
}

void tst_QMcpServerCapabilitiesResources::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpServerCapabilitiesResources capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Test copy constructor
    QMcpServerCapabilitiesResources capabilities2(capabilities);
    QCOMPARE(capabilities2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpServerCapabilitiesResources capabilities3;
    capabilities3 = capabilities2;
    QCOMPARE(capabilities3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpServerCapabilitiesResources)
#include "tst_qmcpservercapabilitiesresources.moc"
