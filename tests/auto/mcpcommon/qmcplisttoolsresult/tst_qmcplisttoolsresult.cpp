// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpListToolsResult>
#include <QtMcpCommon/QMcpTool>
#include <QtTest/QTest>

class tst_QMcpListToolsResult : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpListToolsResult::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Basic result with no cursor
    QTest::newRow("basic") << R"({
        "tools": [
            {
                "name": "tool1",
                "description": "First tool",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "type": "string"
                    }
                }
            }
        ]
    })"_ba
    << QVariantMap {
        { "tools", QVariantList {
            QVariantMap {
                { "name", "tool1" },
                { "description", "First tool" },
                { "inputSchema", QVariantMap {
                    { "type", "object"_ba },
                    { "properties", QVariantMap {
                        { "type", "string" }
                    }}
                }}
            }
        }}
    };

    // Result with cursor and multiple tools
    QTest::newRow("with cursor and tools") << R"({
        "nextCursor": "cursor123",
        "tools": [
            {
                "name": "tool1",
                "description": "First tool",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "additionalProperties": {
                            "type": "string"
                        }
                    }
                }
            },
            {
                "name": "tool2",
                "description": "Second tool",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "additionalProperties": {
                            "type": "string"
                        }
                    }
                }
            }
        ]
    })"_ba
    << QVariantMap {
        { "nextCursor", "cursor123" },
        { "tools", QVariantList {
            QVariantMap {
                { "name", "tool1" },
                { "description", "First tool" },
                { "inputSchema", QVariantMap {
                    { "type", "object"_ba },
                    { "properties", QVariantMap {
                        { "additionalProperties", QVariantMap {
                            { "type", "string" }
                        }}
                    }}
                }}
            },
            QVariantMap {
                { "name", "tool2" },
                { "description", "Second tool" },
                { "inputSchema", QVariantMap {
                    { "type", "object"_ba },
                    { "properties", QVariantMap {
                        { "additionalProperties", QVariantMap {
                            { "type", "string" }
                        }}
                    }}
                }}
            }
        }}
    };

    // Result with empty tools list
    QTest::newRow("empty tools") << R"({
        "nextCursor": "cursor123",
        "tools": []
    })"_ba
    << QVariantMap {
        { "nextCursor", "cursor123" },
        { "tools", QVariantList() }
    };
}

void tst_QMcpListToolsResult::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpListToolsResult result;
    QVERIFY(result.fromJsonObject(object));
    TestHelper::verify(&result, data);

    // Verify conversion
    const auto converted = result.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpListToolsResult::copy_data()
{
    convert_data();
}

void tst_QMcpListToolsResult::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpListToolsResult result;
    QVERIFY(result.fromJsonObject(object));

    // Test copy constructor
    QMcpListToolsResult result2(result);
    QCOMPARE(result2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpListToolsResult result3;
    result3 = result2;
    QCOMPARE(result3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpListToolsResult)
#include "tst_qmcplisttoolsresult.moc"
