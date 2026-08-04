// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpListToolsResult>
#include <QtMcpCommon/QMcpTool>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpListToolsResult : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void cacheHintGating();
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
        { "resultType", "complete"_L1 },
        { "ttlMs", 0 },
        { "cacheScope", "private"_L1 },
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
        { "resultType", "complete"_L1 },
        { "ttlMs", 0 },
        { "cacheScope", "private"_L1 },
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
        { "resultType", "complete"_L1 },
        { "ttlMs", 0 },
        { "cacheScope", "private"_L1 },
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

void tst_QMcpListToolsResult::cacheHintGating()
{
    QMcpListToolsResult result;
    result.setTools({});
    result.setCacheScope("public"_L1);
    result.setTtlMs(60000);

    // tools/list became a CacheableResult in 2026-07-28: neither the caching
    // hints nor Result::resultType may leak into an older revision.
    for (const auto version : { QtMcp::ProtocolVersion::v2024_11_05,
                                QtMcp::ProtocolVersion::v2025_03_26,
                                QtMcp::ProtocolVersion::v2025_06_18,
                                QtMcp::ProtocolVersion::v2025_11_25 }) {
        const auto oldObject = result.toJsonObject(version);
        QVERIFY(!oldObject.contains("resultType"_L1));
        QVERIFY(!oldObject.contains("cacheScope"_L1));
        QVERIFY(!oldObject.contains("ttlMs"_L1));
        // The member that always existed is still there.
        QVERIFY(oldObject.contains("tools"_L1));
    }

    const auto newObject = result.toJsonObject(QtMcp::ProtocolVersion::v2026_07_28);
    QCOMPARE(newObject.value("resultType"_L1).toString(), "complete"_L1);
    QCOMPARE(newObject.value("cacheScope"_L1).toString(), "public"_L1);
    QCOMPARE(newObject.value("ttlMs"_L1).toInt(), 60000);

    // The hints are mandatory on the wire since 2026-07-28, but a server
    // omitting them must not make the result unparsable: it then simply must
    // not be cached.
    QMcpListToolsResult parsed;
    QVERIFY(parsed.fromJsonObject(QJsonObject { { "tools"_L1, QJsonArray {} } },
                                  QtMcp::ProtocolVersion::v2026_07_28));
    QCOMPARE(parsed.resultType(), "complete"_L1);
    QCOMPARE(parsed.cacheScope(), "private"_L1);
    QCOMPARE(parsed.ttlMs(), 0);
}

QTEST_MAIN(tst_QMcpListToolsResult)
#include "tst_qmcplisttoolsresult.moc"
