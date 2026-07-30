// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/qmcpcreatemessagerequestparams.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpCreateMessageRequestParams : public QObject
{
    Q_OBJECT

private:
    // The message a sampling request needs to be valid.
    static QMcpSamplingMessage message();
    // A tool the model may call, with the smallest possible input schema.
    static QMcpTool tool();

private slots:
    void defaultValues();
    void roundTrip();
    void versionGating();
    void parseIgnoresToolsBefore20251125();
    void copy();
};

QMcpSamplingMessage tst_QMcpCreateMessageRequestParams::message()
{
    QMcpTextContent text;
    text.setText("What is the weather?"_L1);

    QMcpSamplingMessage message;
    message.setRole(QMcpRole::user);
    message.setContent(QMcpSamplingMessageContent(text));
    return message;
}

QMcpTool tst_QMcpCreateMessageRequestParams::tool()
{
    QMcpTool tool;
    tool.setName("get_weather"_L1);
    return tool;
}

void tst_QMcpCreateMessageRequestParams::defaultValues()
{
    const QMcpCreateMessageRequestParams params;
    QVERIFY(params.messages().isEmpty());
    QCOMPARE(params.maxTokens(), 0);
    QVERIFY(params.tools().isEmpty());
    // The spec default of toolChoice is { mode: "auto" }, but an unset mode is
    // what tells "the server did not ask for anything" from "auto" apart.
    QVERIFY(params.toolChoice().mode().isEmpty());
}

void tst_QMcpCreateMessageRequestParams::roundTrip()
{
    const auto json = R"({
        "maxTokens": 100,
        "messages": [
            {
                "content": {
                    "type": "text",
                    "text": "What is the weather?"
                },
                "role": "user"
            }
        ],
        "toolChoice": {
            "mode": "required"
        },
        "tools": [
            {
                "name": "get_weather",
                "inputSchema": {
                    "type": "object"
                }
            }
        ]
    })"_ba;

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());
    const auto object = doc.object();

    QMcpCreateMessageRequestParams params;
    QVERIFY(params.fromJsonObject(object));
    QCOMPARE(params.maxTokens(), 100);
    QCOMPARE(params.messages().size(), 1);
    QCOMPARE(params.messages().at(0).content().textContent().text(), "What is the weather?"_L1);
    QCOMPARE(params.tools().size(), 1);
    QCOMPARE(params.tools().at(0).name(), "get_weather"_L1);
    QCOMPARE(params.toolChoice().mode(), "required"_L1);

    // The tools and the tool choice have to survive a round trip through JSON
    // unchanged.
    QCOMPARE(params.toJsonObject(), object);
}

void tst_QMcpCreateMessageRequestParams::versionGating()
{
    QMcpToolChoice toolChoice;
    toolChoice.setMode("required"_L1);

    QMcpCreateMessageRequestParams params;
    params.setMaxTokens(100);
    params.setMessages({ message() });
    params.setTools({ tool() });
    params.setToolChoice(toolChoice);

    // tools and toolChoice have been added in 2025-11-25 and must not leak into
    // an older revision of the protocol.
    const auto oldObject = params.toJsonObject(QtMcp::ProtocolVersion::v2025_06_18);
    QVERIFY(!oldObject.contains("tools"_L1));
    QVERIFY(!oldObject.contains("toolChoice"_L1));
    // The members that already existed are still there.
    QCOMPARE(oldObject.value("maxTokens"_L1).toInt(), 100);
    QCOMPARE(oldObject.value("messages"_L1).toArray().size(), 1);

    const auto newObject = params.toJsonObject(QtMcp::ProtocolVersion::v2025_11_25);
    const auto tools = newObject.value("tools"_L1).toArray();
    QCOMPARE(tools.size(), 1);
    QCOMPARE(tools.at(0).toObject().value("name"_L1).toString(), "get_weather"_L1);
    QCOMPARE(newObject.value("toolChoice"_L1).toObject().value("mode"_L1).toString(),
             "required"_L1);
}

void tst_QMcpCreateMessageRequestParams::parseIgnoresToolsBefore20251125()
{
    const QJsonObject object {
        { "maxTokens"_L1, 100 },
        { "messages"_L1, QJsonArray {} },
        { "toolChoice"_L1, QJsonObject { { "mode"_L1, "required"_L1 } } },
        { "tools"_L1, QJsonArray {
            QJsonObject {
                { "name"_L1, "get_weather"_L1 },
                { "inputSchema"_L1, QJsonObject { { "type"_L1, "object"_L1 } } },
            },
        }},
    };

    // A 2025-06-18 client cannot use them, so they are dropped instead of
    // failing the request.
    QMcpCreateMessageRequestParams params;
    QVERIFY(params.fromJsonObject(object, QtMcp::ProtocolVersion::v2025_06_18));
    QVERIFY(params.tools().isEmpty());
    QVERIFY(params.toolChoice().mode().isEmpty());

    QMcpCreateMessageRequestParams latest;
    QVERIFY(latest.fromJsonObject(object, QtMcp::ProtocolVersion::v2025_11_25));
    QCOMPARE(latest.tools().size(), 1);
    QCOMPARE(latest.toolChoice().mode(), "required"_L1);
}

void tst_QMcpCreateMessageRequestParams::copy()
{
    QMcpToolChoice toolChoice;
    toolChoice.setMode("none"_L1);

    QMcpCreateMessageRequestParams params;
    params.setMaxTokens(100);
    params.setMessages({ message() });
    params.setTools({ tool() });
    params.setToolChoice(toolChoice);

    const auto expected = params.toJsonObject();

    QMcpCreateMessageRequestParams copyConstructed(params);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpCreateMessageRequestParams assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    params.setTools({});
    params.setToolChoice({});
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

QTEST_MAIN(tst_QMcpCreateMessageRequestParams)
#include "tst_qmcpcreatemessagerequestparams.moc"
