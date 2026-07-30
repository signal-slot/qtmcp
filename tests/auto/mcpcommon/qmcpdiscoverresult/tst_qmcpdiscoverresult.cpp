// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpDiscoverResult>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpDiscoverResult : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void versionGating();
    void missingCacheHintsFallBackToDefaults();
};

void tst_QMcpDiscoverResult::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Only the members server/discover has to answer with.
    QTest::newRow("minimal") << R"({
        "resultType": "complete",
        "cacheScope": "private",
        "ttlMs": 0,
        "capabilities": {},
        "supportedVersions": ["2026-07-28"]
    })"_ba
    << QVariantMap {
        { "resultType", "complete"_L1 },
        { "cacheScope", "private"_L1 },
        { "ttlMs", 0 },
        { "capabilities", QVariantMap {} },
        { "supportedVersions", QStringList { "2026-07-28"_L1 } }
    };

    // Every member the 2026-07-28 revision defines for the result, including
    // the caching hints inherited from CacheableResult.
    QTest::newRow("full") << R"({
        "resultType": "complete",
        "cacheScope": "public",
        "ttlMs": 300000,
        "capabilities": {
            "tools": { "listChanged": true },
            "extensions": {
                "io.modelcontextprotocol/tasks": {}
            }
        },
        "instructions": "Ask the echo tool to repeat text",
        "supportedVersions": ["2025-11-25", "2026-07-28"]
    })"_ba
    << QVariantMap {
        { "resultType", "complete"_L1 },
        { "cacheScope", "public"_L1 },
        { "ttlMs", 300000 },
        { "capabilities", QVariantMap {
            { "tools", QVariantMap { { "listChanged", true } } },
            { "extensions", QVariantMap {
                { "io.modelcontextprotocol/tasks", QVariantMap {} }
            }}
        }},
        { "instructions", "Ask the echo tool to repeat text"_L1 },
        { "supportedVersions", QStringList { "2025-11-25"_L1, "2026-07-28"_L1 } }
    };
}

void tst_QMcpDiscoverResult::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpDiscoverResult result;
    QVERIFY(result.fromJsonObject(doc.object()));
    TestHelper::verify(&result, data);

    // The result has to survive a round trip through JSON unchanged.
    QCOMPARE(result.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpDiscoverResult::copy_data()
{
    convert_data();
}

void tst_QMcpDiscoverResult::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpDiscoverResult result;
    QVERIFY(result.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpDiscoverResult copyConstructed(result);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpDiscoverResult assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    result.setInstructions("something else"_L1);
    result.setSupportedVersions({ "2024-11-05"_L1 });
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpDiscoverResult::versionGating()
{
    QMcpDiscoverResult result;
    result.setSupportedVersions({ "2025-11-25"_L1, "2026-07-28"_L1 });
    result.setInstructions("Ask the echo tool to repeat text"_L1);
    result.setCacheScope("public"_L1);
    result.setTtlMs(300000);
    QMcpServerCapabilities capabilities;
    capabilities.setExtensions(QJsonObject {
        { "io.modelcontextprotocol/tasks"_L1, QJsonObject {} }
    });
    result.setCapabilities(capabilities);

    // The whole type only exists since 2026-07-28, but serializing it for an
    // older revision must still drop the members that revision does not know:
    // resultType and the caching hints of CacheableResult.
    const auto oldObject = result.toJsonObject(QtMcp::ProtocolVersion::v2025_11_25);
    QVERIFY(!oldObject.contains("resultType"_L1));
    QVERIFY(!oldObject.contains("cacheScope"_L1));
    QVERIFY(!oldObject.contains("ttlMs"_L1));
    // ServerCapabilities::extensions is gated the same way.
    QVERIFY(!oldObject.value("capabilities"_L1).toObject().contains("extensions"_L1));
    // The members that do not depend on the revision are still there.
    QCOMPARE(oldObject.value("instructions"_L1).toString(), "Ask the echo tool to repeat text"_L1);
    QCOMPARE(oldObject.value("supportedVersions"_L1).toArray().size(), 2);

    const auto newObject = result.toJsonObject(QtMcp::ProtocolVersion::v2026_07_28);
    QCOMPARE(newObject.value("resultType"_L1).toString(), "complete"_L1);
    QCOMPARE(newObject.value("cacheScope"_L1).toString(), "public"_L1);
    QCOMPARE(newObject.value("ttlMs"_L1).toInt(), 300000);
    QVERIFY(newObject.value("capabilities"_L1).toObject().contains("extensions"_L1));
}

void tst_QMcpDiscoverResult::missingCacheHintsFallBackToDefaults()
{
    // resultType, cacheScope and ttlMs are mandatory on the wire since
    // 2026-07-28, but a server omitting them must not make the whole result
    // unparsable: they fall back to "complete" and to not caching at all.
    const auto object = QJsonObject {
        { "capabilities"_L1, QJsonObject {} },
        { "supportedVersions"_L1, QJsonArray { "2026-07-28"_L1 } }
    };

    QMcpDiscoverResult result;
    QVERIFY(result.fromJsonObject(object, QtMcp::ProtocolVersion::v2026_07_28));
    QCOMPARE(result.resultType(), "complete"_L1);
    QCOMPARE(result.cacheScope(), "private"_L1);
    QCOMPARE(result.ttlMs(), 0);
}

QTEST_MAIN(tst_QMcpDiscoverResult)
#include "tst_qmcpdiscoverresult.moc"
