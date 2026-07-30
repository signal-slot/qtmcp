// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonArray>
#include <QtMcpCommon/QMcpTool>
#include <QtMcpCommon/QMcpToolInputSchema>
#include <QtMcpCommon/qmcpicon.h>
#include <QtMcpCommon/qmcptooloutputschema.h>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpTool : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void testVersionGating();
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

void tst_QMcpTool::testVersionGating()
{
    QMcpToolInputSchema inputSchema;
    inputSchema.setProperties(QJsonObject { { "question"_L1, QJsonObject { { "type"_L1, "string"_L1 } } } });

    QMcpToolOutputSchema outputSchema;
    outputSchema.setProperties(QJsonObject { { "answer"_L1, QJsonObject { { "type"_L1, "integer"_L1 } } } });

    QMcpIcon icon;
    icon.setSrc(QUrl("https://example.invalid/tool.png"_L1));
    icon.setSizes({ "48x48"_L1 });

    QMcpTool tool;
    tool.setName("testTool"_L1);
    tool.setInputSchema(inputSchema);
    tool.setTitle("Test Tool"_L1);
    tool.setOutputSchema(outputSchema);
    tool.setIcons({ icon });

    // title and outputSchema have been added in 2025-06-18 and must not leak
    // into an older revision of the protocol.
    const auto oldObject = tool.toJsonObject(QtMcp::ProtocolVersion::v2025_03_26);
    QVERIFY(!oldObject.contains("title"_L1));
    QVERIFY(!oldObject.contains("outputSchema"_L1));
    QVERIFY(!oldObject.contains("icons"_L1));
    // The members that already existed are still there.
    QCOMPARE(oldObject.value("name"_L1).toString(), "testTool"_L1);
    QVERIFY(oldObject.contains("inputSchema"_L1));

    const auto newObject = tool.toJsonObject(QtMcp::ProtocolVersion::v2025_06_18);
    QCOMPARE(newObject.value("title"_L1).toString(), "Test Tool"_L1);
    QVERIFY(newObject.contains("outputSchema"_L1));
    const auto outputSchemaObject = newObject.value("outputSchema"_L1).toObject();
    QCOMPARE(outputSchemaObject.value("type"_L1).toString(), "object"_L1);
    QCOMPARE(outputSchemaObject.value("properties"_L1).toObject(), outputSchema.properties());
    // icons has been added in 2025-11-25 only.
    QVERIFY(!newObject.contains("icons"_L1));

    const auto newestObject = tool.toJsonObject(QtMcp::ProtocolVersion::v2025_11_25);
    const auto icons = newestObject.value("icons"_L1).toArray();
    QCOMPARE(icons.size(), 1);
    QCOMPARE(icons.at(0).toObject().value("src"_L1).toString(),
             "https://example.invalid/tool.png"_L1);
    QCOMPARE(icons.at(0).toObject().value("sizes"_L1).toArray(), QJsonArray { "48x48"_L1 });
    // The members of the older revisions are still there.
    QCOMPARE(newestObject.value("title"_L1).toString(), "Test Tool"_L1);
    QVERIFY(newestObject.contains("outputSchema"_L1));
}

QTEST_MAIN(tst_QMcpTool)
#include "tst_qmcptool.moc"
