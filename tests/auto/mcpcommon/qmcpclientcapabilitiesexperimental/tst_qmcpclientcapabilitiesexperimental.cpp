// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpClientCapabilitiesExperimental>
#include <QtTest/QTest>

class tst_QMcpClientCapabilitiesExperimental : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void settersAndGetters();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpClientCapabilitiesExperimental::defaultValues()
{
    QMcpClientCapabilitiesExperimental capabilities;
    QVERIFY(capabilities.additionalProperties().isEmpty());
}

void tst_QMcpClientCapabilitiesExperimental::settersAndGetters()
{
    QMcpClientCapabilitiesExperimental capabilities;

    // Test with simple properties
    QJsonObject props{
        { "customFeature", true },
        { "clientVersion", "2.0.0" }
    };
    capabilities.setAdditionalProperties(props);
    QCOMPARE(capabilities.additionalProperties(), props);

    // Test with nested properties
    QJsonObject nestedProps{
        { "customFeature", QJsonObject{
            { "enabled", true },
            { "config", QJsonObject{
                { "timeout", 5000 },
                { "maxRetries", 3 }
            }}
        }},
        { "clientInfo", QJsonObject{
            { "version", "2.0.0" },
            { "platform", "Linux" },
            { "supportedFormats", QJsonArray{"text", "json", "binary"} }
        }}
    };
    capabilities.setAdditionalProperties(nestedProps);
    QCOMPARE(capabilities.additionalProperties(), nestedProps);

    // Test setting same values
    capabilities.setAdditionalProperties(nestedProps);
    QCOMPARE(capabilities.additionalProperties(), nestedProps);
}

void tst_QMcpClientCapabilitiesExperimental::convert_data()
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
            "clientVersion": "2.0.0"
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "customFeature", true },
            { "clientVersion", "2.0.0" }
        }}
    };

    // Complex properties
    QTest::newRow("complex") << R"({
        "additionalProperties": {
            "customFeature": {
                "enabled": true,
                "config": {
                    "timeout": 5000,
                    "maxRetries": 3
                }
            },
            "clientInfo": {
                "version": "2.0.0",
                "platform": "Linux",
                "supportedFormats": ["text", "json", "binary"]
            }
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "customFeature", QVariantMap {
                { "enabled", true },
                { "config", QVariantMap {
                    { "timeout", 5000 },
                    { "maxRetries", 3 }
                }}
            }},
            { "clientInfo", QVariantMap {
                { "version", "2.0.0" },
                { "platform", "Linux" },
                { "supportedFormats", QVariantList { "text", "json", "binary" } }
            }}
        }}
    };

    // Client-specific features
    QTest::newRow("client features") << R"({
        "additionalProperties": {
            "ui": {
                "theme": "dark",
                "fontSize": 14,
                "customColors": {
                    "primary": "#007bff",
                    "secondary": "#6c757d"
                }
            },
            "performance": {
                "maxConcurrentRequests": 5,
                "cacheSize": 1024,
                "compressionEnabled": true
            },
            "debug": {
                "logLevel": "debug",
                "metrics": ["cpu", "memory", "network"],
                "tracing": {
                    "enabled": true,
                    "samplingRate": 0.1
                }
            }
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "ui", QVariantMap {
                { "theme", "dark" },
                { "fontSize", 14 },
                { "customColors", QVariantMap {
                    { "primary", "#007bff" },
                    { "secondary", "#6c757d" }
                }}
            }},
            { "performance", QVariantMap {
                { "maxConcurrentRequests", 5 },
                { "cacheSize", 1024 },
                { "compressionEnabled", true }
            }},
            { "debug", QVariantMap {
                { "logLevel", "debug" },
                { "metrics", QVariantList { "cpu", "memory", "network" } },
                { "tracing", QVariantMap {
                    { "enabled", true },
                    { "samplingRate", 0.1 }
                }}
            }}
        }}
    };
}

void tst_QMcpClientCapabilitiesExperimental::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpClientCapabilitiesExperimental capabilities;
    QVERIFY(capabilities.fromJsonObject(object));
    TestHelper::verify(&capabilities, data);

    // Verify conversion
    const auto converted = capabilities.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpClientCapabilitiesExperimental::copy_data()
{
    convert_data();
}

void tst_QMcpClientCapabilitiesExperimental::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpClientCapabilitiesExperimental capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Test copy constructor
    QMcpClientCapabilitiesExperimental capabilities2(capabilities);
    QCOMPARE(capabilities2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpClientCapabilitiesExperimental capabilities3;
    capabilities3 = capabilities2;
    QCOMPARE(capabilities3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpClientCapabilitiesExperimental)
#include "tst_qmcpclientcapabilitiesexperimental.moc"
