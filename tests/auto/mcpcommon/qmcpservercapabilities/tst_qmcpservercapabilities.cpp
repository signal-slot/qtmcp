// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpServerCapabilities>
#include <QtTest/QTest>

class tst_QMcpServerCapabilities : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpServerCapabilities::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Empty capabilities
    QTest::newRow("empty") << R"({
        "experimental": { "additionalProperties": {} },
        "logging": { "additionalProperties": {} },
        "prompts": { "listChanged": false },
        "resources": { "listChanged": false, "subscribe": false },
        "tools": { "listChanged": false }
    })"_ba
    << QVariantMap {
    };

    // All capabilities enabled
    QTest::newRow("all enabled") << R"({
        "experimental": {
            "additionalProperties": {
                "customFeature": true,
                "version": "1.0.0"
            }
        },
        "logging": {
            "additionalProperties": {
                "minLevel": "debug",
                "maxMessageSize": 1024
            }
        },
        "prompts": {
            "listChanged": true
        },
        "resources": {
            "listChanged": true,
            "subscribe": true
        },
        "tools": {
            "listChanged": true
        }
    })"_ba
    << QVariantMap {
        { "experimental", QVariantMap{{"additionalProperties", QVariantMap{
            {"customFeature", true},
            {"version", "1.0.0"}
        }}} },
        { "logging", QVariantMap{{"additionalProperties", QVariantMap{
            {"minLevel", "debug"},
            {"maxMessageSize", 1024}
        }}} },
        { "prompts", QVariantMap{{"listChanged", true}} },
        { "resources", QVariantMap{{"listChanged", true}, {"subscribe", true}} },
        { "tools", QVariantMap{{"listChanged", true}} }
    };

    // Mixed capabilities
    QTest::newRow("mixed") << R"({
        "experimental": {
            "additionalProperties": {
                "customFeature": true
            }
        },
        "logging": {
            "additionalProperties": {}
        },
        "prompts": {
            "listChanged": true
        },
        "resources": {
            "listChanged": false,
            "subscribe": true
        },
        "tools": {
            "listChanged": false
        }
    })"_ba
    << QVariantMap {
        { "experimental", QVariantMap{{"additionalProperties", QVariantMap{
            {"customFeature", true}
        }}} },
        { "prompts", QVariantMap{{"listChanged", true}} },
        { "resources", QVariantMap{{"subscribe", true}} }
    };
}

void tst_QMcpServerCapabilities::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpServerCapabilities capabilities;
    QVERIFY(capabilities.fromJsonObject(object));
    TestHelper::verify(&capabilities, data);

    // Verify conversion
    const auto converted = capabilities.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpServerCapabilities::copy_data()
{
    convert_data();
}

void tst_QMcpServerCapabilities::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpServerCapabilities capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Test copy constructor
    QMcpServerCapabilities capabilities2(capabilities);
    QCOMPARE(capabilities2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpServerCapabilities capabilities3;
    capabilities3 = capabilities2;
    QCOMPARE(capabilities3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpServerCapabilities)
#include "tst_qmcpservercapabilities.moc"
