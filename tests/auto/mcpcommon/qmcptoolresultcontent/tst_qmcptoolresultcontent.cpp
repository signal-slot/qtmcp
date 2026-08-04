// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/qmcpsamplingmessagecontent.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qmcptoolresultcontent.h>
#include <QtTest/QTest>

class tst_QMcpToolResultContent : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void contentBlocks();
    void isErrorDefault();
    void requiredMembers();
    void samplingMessageContent();
};

void tst_QMcpToolResultContent::defaultValues()
{
    const QMcpToolResultContent content;
    // type is constant: a tool result block is always of type "tool_result".
    QCOMPARE(content.type(), "tool_result"_L1);
    QVERIFY(content.toolUseId().isEmpty());
    QVERIFY(content.content().isEmpty());
    QVERIFY(content.structuredContent().isEmpty());
    QCOMPARE(content.isError(), false);
    QVERIFY(content.meta().isEmpty());
}

void tst_QMcpToolResultContent::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // type, toolUseId and content are required; content is serialized even when
    // the tool returned nothing.
    QTest::newRow("minimal") << R"({
        "type": "tool_result",
        "toolUseId": "call-1",
        "content": []
    })"_ba
    << QVariantMap {
        { "type", "tool_result"_L1 },
        { "toolUseId", "call-1"_L1 },
        { "content", QVariantList {} }
    };

    QTest::newRow("text content") << R"({
        "type": "tool_result",
        "toolUseId": "call-2",
        "content": [
            { "type": "text", "text": "22 degrees" }
        ]
    })"_ba
    << QVariantMap {
        { "type", "tool_result"_L1 },
        { "toolUseId", "call-2"_L1 },
        { "content", QVariantList {
            QVariantMap {
                { "type", "text"_L1 },
                { "text", "22 degrees"_L1 }
            }
        }}
    };

    QTest::newRow("structured content") << R"({
        "type": "tool_result",
        "toolUseId": "call-3",
        "content": [
            { "type": "text", "text": "22 degrees" }
        ],
        "structuredContent": {
            "celsius": 22
        }
    })"_ba
    << QVariantMap {
        { "type", "tool_result"_L1 },
        { "toolUseId", "call-3"_L1 },
        { "content", QVariantList {
            QVariantMap {
                { "type", "text"_L1 },
                { "text", "22 degrees"_L1 }
            }
        }},
        { "structuredContent", QVariantMap {
            { "celsius", 22 }
        }}
    };

    QTest::newRow("error") << R"({
        "type": "tool_result",
        "toolUseId": "call-4",
        "content": [
            { "type": "text", "text": "Unknown city" }
        ],
        "isError": true,
        "_meta": {
            "cacheKey": "abc"
        }
    })"_ba
    << QVariantMap {
        { "type", "tool_result"_L1 },
        { "toolUseId", "call-4"_L1 },
        { "content", QVariantList {
            QVariantMap {
                { "type", "text"_L1 },
                { "text", "Unknown city"_L1 }
            }
        }},
        { "isError", true },
        { "_meta", QVariantMap {
            { "cacheKey", "abc"_L1 }
        }}
    };
}

void tst_QMcpToolResultContent::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpToolResultContent content;
    QVERIFY(content.fromJsonObject(object));
    TestHelper::verify(&content, data);

    // The tool result has to survive a round trip through JSON unchanged.
    QCOMPARE(content.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpToolResultContent::copy_data()
{
    convert_data();
}

void tst_QMcpToolResultContent::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpToolResultContent content;
    QVERIFY(content.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpToolResultContent copyConstructed(content);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpToolResultContent assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    content.setToolUseId("other"_L1);
    content.setContent({});
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpToolResultContent::contentBlocks()
{
    // content is the same union as the one of a tool call result, so the blocks
    // are told apart by their "type".
    QMcpToolResultContent content;
    QVERIFY(content.fromJsonObject(QJsonObject {
        { "type"_L1, "tool_result"_L1 },
        { "toolUseId"_L1, "call-1"_L1 },
        { "content"_L1, QJsonArray {
            QJsonObject { { "type"_L1, "text"_L1 }, { "text"_L1, "first"_L1 } },
            QJsonObject { { "type"_L1, "text"_L1 }, { "text"_L1, "second"_L1 } },
        }},
    }));
    QCOMPARE(content.content().size(), 2);
    QCOMPARE(content.content().at(0).refType(), "textContent"_ba);
    QCOMPARE(content.content().at(0).textContent().text(), "first"_L1);
    QCOMPARE(content.content().at(1).textContent().text(), "second"_L1);

    // Blocks set from C++ end up in the array in the same order.
    QMcpTextContent text;
    text.setText("third"_L1);
    QMcpToolResultContent built;
    built.setToolUseId("call-2"_L1);
    built.setContent({ QMcpCallToolResultContent(text) });
    const auto array = built.toJsonObject().value("content"_L1).toArray();
    QCOMPARE(array.size(), 1);
    QCOMPARE(array.at(0).toObject().value("text"_L1).toString(), "third"_L1);
    QCOMPARE(array.at(0).toObject().value("type"_L1).toString(), "text"_L1);
}

void tst_QMcpToolResultContent::isErrorDefault()
{
    QMcpToolResultContent content;
    content.setToolUseId("call-1"_L1);
    // false is the default, so it is not serialized at all.
    QVERIFY(!content.toJsonObject().contains("isError"_L1));

    content.setIsError(true);
    QCOMPARE(content.toJsonObject().value("isError"_L1).toBool(), true);

    // Parsing an explicit false leaves the default in place.
    QMcpToolResultContent parsed;
    QVERIFY(parsed.fromJsonObject(QJsonObject {
        { "type"_L1, "tool_result"_L1 },
        { "toolUseId"_L1, "call-1"_L1 },
        { "content"_L1, QJsonArray {} },
        { "isError"_L1, false },
    }));
    QCOMPARE(parsed.isError(), false);
    QVERIFY(!parsed.toJsonObject().contains("isError"_L1));
}

void tst_QMcpToolResultContent::requiredMembers()
{
    const QJsonObject complete {
        { "type"_L1, "tool_result"_L1 },
        { "toolUseId"_L1, "call-1"_L1 },
        { "content"_L1, QJsonArray {} },
    };
    QMcpToolResultContent content;
    QVERIFY(content.fromJsonObject(complete));

    for (const auto &key : { "toolUseId"_L1, "content"_L1 }) {
        auto incomplete = complete;
        incomplete.remove(key);
        QMcpToolResultContent broken;
        QVERIFY2(!broken.fromJsonObject(incomplete), qPrintable(key));
    }
}

void tst_QMcpToolResultContent::samplingMessageContent()
{
    // Since 2025-11-25 a sampling message may carry the result of a tool use.
    const QJsonObject object {
        { "type"_L1, "tool_result"_L1 },
        { "toolUseId"_L1, "call-1"_L1 },
        { "content"_L1, QJsonArray {
            QJsonObject { { "type"_L1, "text"_L1 }, { "text"_L1, "22 degrees"_L1 } },
        }},
    };

    QMcpSamplingMessageContent content;
    QVERIFY(content.fromJsonObject(object));
    QCOMPARE(content.refType(), "toolResult"_ba);
    QCOMPARE(content.toolResult().toolUseId(), "call-1"_L1);
    QCOMPARE(content.toolResult().content().size(), 1);
    QCOMPARE(content.toJsonObject(), object);

    // The convenience constructor selects the variant as well.
    QMcpToolResultContent toolResult;
    toolResult.setToolUseId("call-2"_L1);
    const QMcpSamplingMessageContent constructed(toolResult);
    QCOMPARE(constructed.refType(), "toolResult"_ba);
    QCOMPARE(constructed.toJsonObject().value("toolUseId"_L1).toString(), "call-2"_L1);
}

QTEST_MAIN(tst_QMcpToolResultContent)
#include "tst_qmcptoolresultcontent.moc"
