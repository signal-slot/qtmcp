// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpTool>
#include <QtMcpCommon/QMcpToolInputSchema>
#include <QtTest/QTest>

class tst_QMcpTool : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpTool::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Basic tool
    QTest::newRow("basic") << R"({
        "name": "testTool",
        "inputSchema": {
            "type": "object",
            "properties": {
                "type": "string"
            }
        }
    })"_ba
    << QVariantMap {
        { "name", "testTool" },
        { "inputSchema", QVariantMap {
            { "type", "object"_ba },
            { "properties", QVariantMap {
                { "type", "string" }
            }}
        }}
    };

    // Tool with description
    QTest::newRow("with description") << R"({
        "name": "testTool",
        "description": "A test tool",
        "inputSchema": {
            "type": "object",
            "properties": {
                "message": {
                    "type": "string",
                    "description": "The message to process"
                }
            },
            "required": ["message"]
        }
    })"_ba
    << QVariantMap {
        { "name", "testTool" },
        { "description", "A test tool" },
        { "inputSchema", QVariantMap {
            { "type", "object"_ba },
            { "properties", QVariantMap {
                { "message", QVariantMap {
                    { "type", "string" },
                    { "description", "The message to process" }
                }}
            }},
            { "required", QStringList { "message" } }
        }}
    };

    // Tool with empty description
    QTest::newRow("empty description") << R"({
        "name": "testTool",
        "description": "",
        "inputSchema": {
            "type": "object",
            "properties": {
                "type": "string"
            }
        }
    })"_ba
    << QVariantMap {
        { "name", "testTool" },
        { "inputSchema", QVariantMap {
            { "type", "object"_ba },
            { "properties", QVariantMap {
                { "type", "string" }
            }}
        }}
    };
}

void tst_QMcpTool::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpTool tool;
    QVERIFY(tool.fromJsonObject(object));
    TestHelper::verify(&tool, data);

    // Verify conversion
    const auto converted = tool.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpTool::copy_data()
{
    convert_data();
}

void tst_QMcpTool::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpTool tool;
    QVERIFY(tool.fromJsonObject(object));

    // Test copy constructor
    QMcpTool tool2(tool);
    QCOMPARE(tool2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpTool tool3;
    tool3 = tool2;
    QCOMPARE(tool3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpTool)
#include "tst_qmcptool.moc"
