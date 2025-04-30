// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpInitializeResult>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpInitializeResult : public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpInitializeResult::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with full capabilities and instructions
    QTest::newRow("full result") << R"({
        "capabilities": {
            "experimental": {
                "additionalProperties": {
                    "feature1": true,
                    "feature2": {
                        "nested": "value"
                    }
                }
            },
            "logging": {
                "level": true
            },
            "prompts": {
                "listChanged": true
            },
            "resources": {
                "listChanged": true
            },
            "tools": {
                "listChanged": true
            }
        },
        "instructions": "Server provides completion, resource access, and tool execution capabilities.",
        "protocolVersion": "2024-11-05",
        "serverInfo": {
            "name": "TestServer",
            "version": "1.0.0"
        }
    })"_ba
    << QVariantMap {
        { "capabilities", QVariantMap {
            { "experimental", QVariantMap {
                { "additionalProperties", QVariantMap {
                    { "feature1", true },
                    { "feature2", QVariantMap {
                        { "nested", "value"_L1 }
                    }}
                }}
            }},
            { "prompts", QVariantMap {
                { "listChanged", true }
            }},
            { "resources", QVariantMap {
                { "listChanged", true },
            }},
            { "tools", QVariantMap {
                { "listChanged", true }
            }}
        }},
        { "instructions", "Server provides completion, resource access, and tool execution capabilities."_L1 },
        { "protocolVersion", "2024-11-05"_L1 },
        { "serverInfo", QVariantMap {
            { "name", "TestServer"_L1 },
            { "version", "1.0.0"_L1 }
        }}
    };

    // Test with minimal capabilities
    QTest::newRow("minimal result") << R"({
        "capabilities": {
            "experimental": {},
            "logging": {
                "level": false
            }
        },
        "protocolVersion": "2024-11-05",
        "serverInfo": {
            "name": "MinimalServer",
            "version": "0.1.0"
        }
    })"_ba
    << QVariantMap {
        { "capabilities", QVariantMap {
        }},
        { "protocolVersion", "2024-11-05"_L1 },
        { "serverInfo", QVariantMap {
            { "name", "MinimalServer"_L1 },
            { "version", "0.1.0"_L1 }
        }}
    };
}

void tst_QMcpInitializeResult::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpInitializeResult result;
    QVERIFY(result.fromJsonObject(object));
    TestHelper::verify(&result, data);

    const auto objectConverted = result.toJsonObject();
    QCOMPARE(objectConverted, QJsonObject::fromVariantMap(data));
}

void tst_QMcpInitializeResult::copy_data()
{
    convert_data();
}

void tst_QMcpInitializeResult::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpInitializeResult result;
    QVERIFY(result.fromJsonObject(object));

    QMcpInitializeResult result2(result);
    TestHelper::verify(&result2, data);

    QMcpInitializeResult result3;
    result3 = result2;
    TestHelper::verify(&result3, data);
}

QTEST_MAIN(tst_QMcpInitializeResult)
#include "tst_qmcpinitializeresult.moc"
