// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpServerCapabilitiesExperimental>
#include <QtTest/QTest>

class tst_QMcpServerCapabilitiesExperimental : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpServerCapabilitiesExperimental::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Empty properties
    QTest::newRow("empty") << R"({
    })"_ba
    << QVariantMap {
    };

    // Simple properties
    QTest::newRow("simple") << R"({
        "additionalProperties": {
            "customFeature": true,
            "version": "1.0.0"
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "customFeature", true },
            { "version", "1.0.0" }
        }}
    };

    // Complex properties
    QTest::newRow("complex") << R"({
        "additionalProperties": {
            "customFeature": {
                "enabled": true,
                "config": {
                    "timeout": 1000,
                    "retries": 3
                }
            },
            "version": "1.0.0"
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "customFeature", QVariantMap {
                { "enabled", true },
                { "config", QVariantMap {
                    { "timeout", 1000 },
                    { "retries", 3 }
                }}
            }},
            { "version", "1.0.0" }
        }}
    };

    // Array properties
    QTest::newRow("array") << R"({
        "additionalProperties": {
            "supportedFeatures": ["feature1", "feature2"],
            "limits": {
                "maxItems": 100,
                "allowedTypes": ["type1", "type2"]
            }
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "supportedFeatures", QVariantList { "feature1", "feature2" } },
            { "limits", QVariantMap {
                { "maxItems", 100 },
                { "allowedTypes", QVariantList { "type1", "type2" } }
            }}
        }}
    };
}

void tst_QMcpServerCapabilitiesExperimental::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpServerCapabilitiesExperimental capabilities;
    QVERIFY(capabilities.fromJsonObject(object));
    TestHelper::verify(&capabilities, data);

    // Verify conversion
    const auto converted = capabilities.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpServerCapabilitiesExperimental::copy_data()
{
    convert_data();
}

void tst_QMcpServerCapabilitiesExperimental::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpServerCapabilitiesExperimental capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Test copy constructor
    QMcpServerCapabilitiesExperimental capabilities2(capabilities);
    QCOMPARE(capabilities2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpServerCapabilitiesExperimental capabilities3;
    capabilities3 = capabilities2;
    QCOMPARE(capabilities3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpServerCapabilitiesExperimental)
#include "tst_qmcpservercapabilitiesexperimental.moc"
