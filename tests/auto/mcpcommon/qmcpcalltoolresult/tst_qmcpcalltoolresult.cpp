// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/qmcpcalltoolresult.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpCallToolResult : public QObject
{
    Q_OBJECT

private:
    // A result carrying a single text content, used by the tests that are only
    // interested in the other members.
    static QMcpCallToolResult resultWithText(const QString &text);

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void versionGating();
    void resultTypeGating();
};

QMcpCallToolResult tst_QMcpCallToolResult::resultWithText(const QString &text)
{
    QMcpTextContent textContent;
    textContent.setText(text);

    QMcpCallToolResult result;
    result.setContent({ QMcpCallToolResultContent(textContent) });
    return result;
}

void tst_QMcpCallToolResult::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    QTest::newRow("text content") << R"({
        "content": [
            {
                "type": "text",
                "text": "42"
            }
        ]
    })"_ba
    << QVariantMap {
        { "resultType", "complete"_L1 },
        { "content", QVariantList {
            QVariantMap {
                { "type", "text"_L1 },
                { "text", "42"_L1 }
            }
        }}
    };

    QTest::newRow("structured content") << R"({
        "content": [
            {
                "type": "text",
                "text": "42"
            }
        ],
        "structuredContent": {
            "answer": 42
        }
    })"_ba
    << QVariantMap {
        { "resultType", "complete"_L1 },
        { "content", QVariantList {
            QVariantMap {
                { "type", "text"_L1 },
                { "text", "42"_L1 }
            }
        }},
        { "structuredContent", QVariantMap {
            { "answer", 42 }
        }}
    };

    QTest::newRow("error") << R"({
        "content": [
            {
                "type": "text",
                "text": "the tool failed"
            }
        ],
        "isError": true
    })"_ba
    << QVariantMap {
        { "resultType", "complete"_L1 },
        { "content", QVariantList {
            QVariantMap {
                { "type", "text"_L1 },
                { "text", "the tool failed"_L1 }
            }
        }},
        { "isError", true }
    };
}

void tst_QMcpCallToolResult::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpCallToolResult result;
    QVERIFY(result.fromJsonObject(doc.object()));
    TestHelper::verify(&result, data);

    QCOMPARE(result.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpCallToolResult::copy_data()
{
    convert_data();
}

void tst_QMcpCallToolResult::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpCallToolResult result;
    QVERIFY(result.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpCallToolResult copyConstructed(result);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpCallToolResult assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    result.setIsError(!result.isError());
    result.setStructuredContent(QJsonObject { { "changed"_L1, true } });
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpCallToolResult::versionGating()
{
    auto result = resultWithText("42"_L1);
    result.setStructuredContent(QJsonObject { { "answer"_L1, 42 } });

    // structuredContent has been added in 2025-06-18 and must not leak into an
    // older revision of the protocol.
    const auto oldObject = result.toJsonObject(QtMcp::ProtocolVersion::v2025_03_26);
    QVERIFY(!oldObject.contains("structuredContent"_L1));
    // The members that already existed are still there.
    QVERIFY(oldObject.contains("content"_L1));

    const auto newObject = result.toJsonObject(QtMcp::ProtocolVersion::v2025_06_18);
    QVERIFY(newObject.contains("structuredContent"_L1));
    QCOMPARE(newObject.value("structuredContent"_L1).toObject().value("answer"_L1).toInt(), 42);
}

void tst_QMcpCallToolResult::resultTypeGating()
{
    auto result = resultWithText("42"_L1);

    // Result::resultType has been added in 2026-07-28: every result carries it
    // from then on, and no result may carry it before that.
    QVERIFY(!result.toJsonObject(QtMcp::ProtocolVersion::v2024_11_05).contains("resultType"_L1));
    QVERIFY(!result.toJsonObject(QtMcp::ProtocolVersion::v2025_06_18).contains("resultType"_L1));
    QVERIFY(!result.toJsonObject(QtMcp::ProtocolVersion::v2025_11_25).contains("resultType"_L1));

    const auto newObject = result.toJsonObject(QtMcp::ProtocolVersion::v2026_07_28);
    QCOMPARE(newObject.value("resultType"_L1).toString(), "complete"_L1);

    // A 2026-07-28 server that omits the mandatory member still has to be
    // understood, and an absent resultType means "complete".
    QMcpCallToolResult parsed;
    QVERIFY(parsed.fromJsonObject(QJsonObject { { "content"_L1, QJsonArray {} } },
                                  QtMcp::ProtocolVersion::v2026_07_28));
    QCOMPARE(parsed.resultType(), "complete"_L1);
}

QTEST_MAIN(tst_QMcpCallToolResult)
#include "tst_qmcpcalltoolresult.moc"
