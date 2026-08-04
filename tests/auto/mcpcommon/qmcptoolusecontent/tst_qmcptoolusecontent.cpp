// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/qmcpsamplingmessagecontent.h>
#include <QtMcpCommon/qmcptoolusecontent.h>
#include <QtTest/QTest>

class tst_QMcpToolUseContent : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void requiredMembers();
    void samplingMessageContent();
};

void tst_QMcpToolUseContent::defaultValues()
{
    const QMcpToolUseContent content;
    // type is constant: a tool use block is always of type "tool_use".
    QCOMPARE(content.type(), "tool_use"_L1);
    QVERIFY(content.id().isEmpty());
    QVERIFY(content.name().isEmpty());
    QVERIFY(content.input().isEmpty());
    QVERIFY(content.meta().isEmpty());
}

void tst_QMcpToolUseContent::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // The required members: type, id, name and input. input is serialized even
    // when the tool takes no arguments.
    QTest::newRow("minimal") << R"({
        "type": "tool_use",
        "id": "call-1",
        "name": "get_time",
        "input": {}
    })"_ba
    << QVariantMap {
        { "type", "tool_use"_L1 },
        { "id", "call-1"_L1 },
        { "name", "get_time"_L1 },
        { "input", QVariantMap {} }
    };

    QTest::newRow("with input") << R"({
        "type": "tool_use",
        "id": "call-2",
        "name": "get_weather",
        "input": {
            "city": "Tokyo",
            "days": 3
        }
    })"_ba
    << QVariantMap {
        { "type", "tool_use"_L1 },
        { "id", "call-2"_L1 },
        { "name", "get_weather"_L1 },
        { "input", QVariantMap {
            { "city", "Tokyo"_L1 },
            { "days", 3 }
        }}
    };

    // _meta is meant to be passed back unchanged in the following sampling
    // request, so it has to survive the round trip.
    QTest::newRow("with _meta") << R"({
        "type": "tool_use",
        "id": "call-3",
        "name": "get_weather",
        "input": {
            "city": "Tokyo"
        },
        "_meta": {
            "cacheKey": "abc"
        }
    })"_ba
    << QVariantMap {
        { "type", "tool_use"_L1 },
        { "id", "call-3"_L1 },
        { "name", "get_weather"_L1 },
        { "input", QVariantMap {
            { "city", "Tokyo"_L1 }
        }},
        { "_meta", QVariantMap {
            { "cacheKey", "abc"_L1 }
        }}
    };
}

void tst_QMcpToolUseContent::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpToolUseContent content;
    QVERIFY(content.fromJsonObject(object));
    TestHelper::verify(&content, data);

    // The tool use has to survive a round trip through JSON unchanged.
    QCOMPARE(content.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpToolUseContent::copy_data()
{
    convert_data();
}

void tst_QMcpToolUseContent::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpToolUseContent content;
    QVERIFY(content.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpToolUseContent copyConstructed(content);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpToolUseContent assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    content.setId("other"_L1);
    content.setInput(QJsonObject { { "city"_L1, "Osaka"_L1 } });
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpToolUseContent::requiredMembers()
{
    // id, name and input are all REQUIRED; a block missing any of them cannot
    // be turned into a tool call.
    const QJsonObject complete {
        { "type"_L1, "tool_use"_L1 },
        { "id"_L1, "call-1"_L1 },
        { "name"_L1, "get_time"_L1 },
        { "input"_L1, QJsonObject {} },
    };
    QMcpToolUseContent content;
    QVERIFY(content.fromJsonObject(complete));

    for (const auto &key : { "id"_L1, "name"_L1, "input"_L1 }) {
        auto incomplete = complete;
        incomplete.remove(key);
        QMcpToolUseContent broken;
        QVERIFY2(!broken.fromJsonObject(incomplete), qPrintable(key));
    }

    // type is constant, so it is not read from the JSON but always written.
    auto withoutType = complete;
    withoutType.remove("type"_L1);
    QMcpToolUseContent parsed;
    QVERIFY(parsed.fromJsonObject(withoutType));
    QCOMPARE(parsed.toJsonObject().value("type"_L1).toString(), "tool_use"_L1);
}

void tst_QMcpToolUseContent::samplingMessageContent()
{
    // Since 2025-11-25 a sampling message may carry a tool use block. The union
    // picks the variant by the "type" member.
    const QJsonObject object {
        { "type"_L1, "tool_use"_L1 },
        { "id"_L1, "call-1"_L1 },
        { "name"_L1, "get_weather"_L1 },
        { "input"_L1, QJsonObject { { "city"_L1, "Tokyo"_L1 } } },
    };

    QMcpSamplingMessageContent content;
    QVERIFY(content.fromJsonObject(object));
    QCOMPARE(content.refType(), "toolUse"_ba);
    QCOMPARE(content.toolUse().id(), "call-1"_L1);
    QCOMPARE(content.toolUse().name(), "get_weather"_L1);
    QCOMPARE(content.toolUse().input().value("city"_L1).toString(), "Tokyo"_L1);
    QCOMPARE(content.toJsonObject(), object);

    // The convenience constructor selects the variant as well.
    QMcpToolUseContent toolUse;
    toolUse.setId("call-2"_L1);
    toolUse.setName("get_time"_L1);
    const QMcpSamplingMessageContent constructed(toolUse);
    QCOMPARE(constructed.refType(), "toolUse"_ba);
    QCOMPARE(constructed.toJsonObject().value("id"_L1).toString(), "call-2"_L1);
}

QTEST_MAIN(tst_QMcpToolUseContent)
#include "tst_qmcptoolusecontent.moc"
