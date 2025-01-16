// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpClientCapabilitiesSampling>
#include <QtTest/QTest>

class tst_QMcpClientCapabilitiesSampling : public QObject
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

void tst_QMcpClientCapabilitiesSampling::defaultValues()
{
    QMcpClientCapabilitiesSampling capabilities;
    QVERIFY(capabilities.additionalProperties().isEmpty());
}

void tst_QMcpClientCapabilitiesSampling::settersAndGetters()
{
    QMcpClientCapabilitiesSampling capabilities;

    // Test with simple properties
    QJsonObject props{
        { "maxTokens", 2048 },
        { "temperature", 0.7 }
    };
    capabilities.setAdditionalProperties(props);
    QCOMPARE(capabilities.additionalProperties(), props);

    // Test with nested properties
    QJsonObject nestedProps{
        { "sampling", QJsonObject{
            { "maxTokens", 2048 },
            { "temperature", 0.7 },
            { "topP", 0.9 },
            { "frequencyPenalty", 0.5 },
            { "presencePenalty", 0.5 }
        }},
        { "models", QJsonArray{
            "gpt-4",
            "gpt-3.5-turbo",
            "claude-2"
        }},
        { "features", QJsonObject{
            { "streaming", true },
            { "functionCalling", true },
            { "systemPrompts", true }
        }}
    };
    capabilities.setAdditionalProperties(nestedProps);
    QCOMPARE(capabilities.additionalProperties(), nestedProps);

    // Test setting same values
    capabilities.setAdditionalProperties(nestedProps);
    QCOMPARE(capabilities.additionalProperties(), nestedProps);
}

void tst_QMcpClientCapabilitiesSampling::convert_data()
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
            "maxTokens": 2048,
            "temperature": 0.7
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "maxTokens", 2048 },
            { "temperature", 0.7 }
        }}
    };

    // Complex properties
    QTest::newRow("complex") << R"({
        "additionalProperties": {
            "sampling": {
                "maxTokens": 2048,
                "temperature": 0.7,
                "topP": 0.9,
                "frequencyPenalty": 0.5,
                "presencePenalty": 0.5
            },
            "models": ["gpt-4", "gpt-3.5-turbo", "claude-2"],
            "features": {
                "streaming": true,
                "functionCalling": true,
                "systemPrompts": true
            }
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "sampling", QVariantMap {
                { "maxTokens", 2048 },
                { "temperature", 0.7 },
                { "topP", 0.9 },
                { "frequencyPenalty", 0.5 },
                { "presencePenalty", 0.5 }
            }},
            { "models", QVariantList { "gpt-4", "gpt-3.5-turbo", "claude-2" } },
            { "features", QVariantMap {
                { "streaming", true },
                { "functionCalling", true },
                { "systemPrompts", true }
            }}
        }}
    };

    // Advanced sampling configuration
    QTest::newRow("advanced") << R"({
        "additionalProperties": {
            "sampling": {
                "maxTokens": 4096,
                "temperature": 0.8,
                "topP": 0.95,
                "frequencyPenalty": 0.7,
                "presencePenalty": 0.7,
                "stopSequences": ["\n\n", "###", "```"],
                "logitBias": {
                    "50256": -100,
                    "50257": -100
                }
            },
            "models": {
                "default": "gpt-4",
                "available": ["gpt-4", "gpt-3.5-turbo", "claude-2"],
                "configurations": {
                    "gpt-4": {
                        "maxTokens": 8192,
                        "contextWindow": 8192
                    },
                    "gpt-3.5-turbo": {
                        "maxTokens": 4096,
                        "contextWindow": 4096
                    }
                }
            },
            "features": {
                "streaming": true,
                "functionCalling": true,
                "systemPrompts": true,
                "toolCalling": true,
                "visionModality": true,
                "codeCompletion": true
            }
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "sampling", QVariantMap {
                { "maxTokens", 4096 },
                { "temperature", 0.8 },
                { "topP", 0.95 },
                { "frequencyPenalty", 0.7 },
                { "presencePenalty", 0.7 },
                { "stopSequences", QVariantList { "\n\n", "###", "```" } },
                { "logitBias", QVariantMap {
                    { "50256", -100 },
                    { "50257", -100 }
                }}
            }},
            { "models", QVariantMap {
                { "default", "gpt-4" },
                { "available", QVariantList { "gpt-4", "gpt-3.5-turbo", "claude-2" } },
                { "configurations", QVariantMap {
                    { "gpt-4", QVariantMap {
                        { "maxTokens", 8192 },
                        { "contextWindow", 8192 }
                    }},
                    { "gpt-3.5-turbo", QVariantMap {
                        { "maxTokens", 4096 },
                        { "contextWindow", 4096 }
                    }}
                }}
            }},
            { "features", QVariantMap {
                { "streaming", true },
                { "functionCalling", true },
                { "systemPrompts", true },
                { "toolCalling", true },
                { "visionModality", true },
                { "codeCompletion", true }
            }}
        }}
    };
}

void tst_QMcpClientCapabilitiesSampling::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpClientCapabilitiesSampling capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Verify conversion
    const auto converted = capabilities.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpClientCapabilitiesSampling::copy_data()
{
    convert_data();
}

void tst_QMcpClientCapabilitiesSampling::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpClientCapabilitiesSampling capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Test copy constructor
    QMcpClientCapabilitiesSampling capabilities2(capabilities);
    QCOMPARE(capabilities2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpClientCapabilitiesSampling capabilities3;
    capabilities3 = capabilities2;
    QCOMPARE(capabilities3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpClientCapabilitiesSampling)
#include "tst_qmcpclientcapabilitiessampling.moc"
