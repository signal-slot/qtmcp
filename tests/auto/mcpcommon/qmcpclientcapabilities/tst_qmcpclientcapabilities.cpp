// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpClientCapabilities>
#include <QtTest/QTest>

class tst_QMcpClientCapabilities : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpClientCapabilities::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Empty capabilities
    QTest::newRow("empty") << R"({
    })"_ba
    << QVariantMap {
    };

    // All capabilities enabled
    QTest::newRow("all enabled") << R"({
        "experimental": {
            "additionalProperties": {
                "customFeature": true,
                "clientVersion": "2.0.0",
                "clientInfo": {
                    "platform": "Linux",
                    "supportedFormats": ["text", "json"]
                }
            }
        },
        "roots": {
            "listChanged": true
        },
        "sampling": {
            "additionalProperties": {
                "sampling": {
                    "maxTokens": 2048,
                    "temperature": 0.7,
                    "topP": 0.9,
                    "frequencyPenalty": 0.5,
                    "presencePenalty": 0.5
                },
                "models": ["gpt-4", "gpt-3.5-turbo"],
                "features": {
                    "streaming": true,
                    "functionCalling": true,
                    "systemPrompts": true
                }
            }
        }
    })"_ba
    << QVariantMap {
        { "experimental", QVariantMap{{"additionalProperties", QVariantMap{
            {"customFeature", true},
            {"clientVersion", "2.0.0"},
            {"clientInfo", QVariantMap{
                {"platform", "Linux"},
                {"supportedFormats", QVariantList{"text", "json"}}
            }}
        }}} },
        { "roots", QVariantMap{{"listChanged", true}} },
        { "sampling", QVariantMap{{"additionalProperties", QVariantMap{
            {"sampling", QVariantMap{
                {"maxTokens", 2048},
                {"temperature", 0.7},
                {"topP", 0.9},
                {"frequencyPenalty", 0.5},
                {"presencePenalty", 0.5}
            }},
            {"models", QVariantList{"gpt-4", "gpt-3.5-turbo"}},
            {"features", QVariantMap{
                {"streaming", true},
                {"functionCalling", true},
                {"systemPrompts", true}
            }}
        }}} }
    };

    // Mixed capabilities
    QTest::newRow("mixed") << R"({
        "experimental": {
            "additionalProperties": {
                "customFeature": true
            }
        },
        "sampling": {
            "additionalProperties": {
                "maxTokens": 2048,
                "temperature": 0.7
            }
        }
    })"_ba
    << QVariantMap {
        { "experimental", QVariantMap{{"additionalProperties", QVariantMap{
            {"customFeature", true}
        }}} },
        { "sampling", QVariantMap{{"additionalProperties", QVariantMap{
            {"maxTokens", 2048},
            {"temperature", 0.7}
        }}} }
    };
}

void tst_QMcpClientCapabilities::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpClientCapabilities capabilities;
    QVERIFY(capabilities.fromJsonObject(object));
    TestHelper::verify(&capabilities, data);

    // Verify conversion
    const auto converted = capabilities.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpClientCapabilities::copy_data()
{
    convert_data();
}

void tst_QMcpClientCapabilities::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpClientCapabilities capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Test copy constructor
    QMcpClientCapabilities capabilities2(capabilities);
    QCOMPARE(capabilities2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpClientCapabilities capabilities3;
    capabilities3 = capabilities2;
    QCOMPARE(capabilities3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpClientCapabilities)
#include "tst_qmcpclientcapabilities.moc"
