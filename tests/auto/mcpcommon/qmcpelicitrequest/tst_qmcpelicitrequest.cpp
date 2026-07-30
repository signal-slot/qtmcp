// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/qmcpelicitrequest.h>
#include <QtTest/QTest>

class tst_QMcpElicitRequest : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void method();
    void urlMode();
};

void tst_QMcpElicitRequest::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // A schema exercising all four PrimitiveSchemaDefinition variants.
    QTest::newRow("all primitive schemas") << R"({
        "id": 42,
        "params": {
            "message": "Please tell us about yourself",
            "requestedSchema": {
                "type": "object",
                "properties": {
                    "agree": {
                        "type": "boolean",
                        "title": "Agree",
                        "description": "Accept the terms",
                        "default": true
                    },
                    "nickname": {
                        "type": "string",
                        "title": "Nickname",
                        "minLength": 1,
                        "maxLength": 32
                    },
                    "age": {
                        "type": "integer",
                        "title": "Age",
                        "minimum": 0,
                        "maximum": 150
                    },
                    "color": {
                        "type": "string",
                        "title": "Favorite color",
                        "enum": ["red", "green", "blue"],
                        "enumNames": ["Red", "Green", "Blue"]
                    }
                },
                "required": ["agree", "nickname"]
            }
        }
    })"_ba
    << QVariantMap {
        { "id", 42 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "elicitation/create"_L1 },
        { "params", QVariantMap {
            { "message", "Please tell us about yourself"_L1 },
            { "requestedSchema", QVariantMap {
                { "type", "object"_ba },
                { "properties", QVariantMap {
                    { "agree", QVariantMap {
                        { "type", "boolean"_L1 },
                        { "title", "Agree"_L1 },
                        { "description", "Accept the terms"_L1 },
                        { "default", true }
                    }},
                    { "nickname", QVariantMap {
                        { "type", "string"_L1 },
                        { "title", "Nickname"_L1 },
                        { "minLength", 1 },
                        { "maxLength", 32 }
                    }},
                    { "age", QVariantMap {
                        { "type", "integer"_L1 },
                        { "title", "Age"_L1 },
                        { "minimum", 0 },
                        { "maximum", 150 }
                    }},
                    { "color", QVariantMap {
                        { "type", "string"_L1 },
                        { "title", "Favorite color"_L1 },
                        { "enum", QStringList { "red"_L1, "green"_L1, "blue"_L1 } },
                        { "enumNames", QStringList { "Red"_L1, "Green"_L1, "Blue"_L1 } }
                    }}
                }},
                { "required", QStringList { "agree"_L1, "nickname"_L1 } }
            }}
        }}
    };

    // Only the members the protocol requires.
    QTest::newRow("minimal") << R"({
        "id": "req-1",
        "params": {
            "message": "Confirm?",
            "requestedSchema": {
                "type": "object",
                "properties": {}
            }
        }
    })"_ba
    << QVariantMap {
        { "id", "req-1"_L1 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "elicitation/create"_L1 },
        { "params", QVariantMap {
            { "message", "Confirm?"_L1 },
            { "requestedSchema", QVariantMap {
                { "type", "object"_ba },
                { "properties", QVariantMap {} }
            }}
        }}
    };

    // In "url" mode the user is sent to a URL instead of being shown a form, so
    // there is no requestedSchema.
    QTest::newRow("url mode") << R"({
        "id": "req-2",
        "params": {
            "mode": "url",
            "message": "Sign in to continue",
            "url": "https://example.invalid/consent",
            "elicitationId": "elicit-1"
        }
    })"_ba
    << QVariantMap {
        { "id", "req-2"_L1 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "elicitation/create"_L1 },
        { "params", QVariantMap {
            { "mode", "url"_L1 },
            { "message", "Sign in to continue"_L1 },
            { "url", "https://example.invalid/consent"_L1 },
            { "elicitationId", "elicit-1"_L1 }
        }}
    };
}

void tst_QMcpElicitRequest::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpElicitRequest request;
    QVERIFY(request.fromJsonObject(doc.object()));
    TestHelper::verify(&request, data);

    QCOMPARE(request.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpElicitRequest::copy_data()
{
    convert_data();
}

void tst_QMcpElicitRequest::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpElicitRequest request;
    QVERIFY(request.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpElicitRequest copyConstructed(request);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpElicitRequest assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    auto params = request.params();
    params.setMessage("changed"_L1);
    request.setParams(params);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpElicitRequest::method()
{
    // The method is constant and part of every serialization.
    const QMcpElicitRequest request;
    QCOMPARE(request.method(), "elicitation/create"_L1);
}

void tst_QMcpElicitRequest::urlMode()
{
    QMcpElicitRequestParams params;
    params.setMode("url"_L1);
    params.setMessage("Sign in to continue"_L1);
    params.setUrl(QUrl("https://example.invalid/consent"_L1));
    params.setElicitationId("elicit-1"_L1);

    const auto latest = params.toJsonObject();
    QCOMPARE(latest.value("mode"_L1).toString(), "url"_L1);
    QCOMPARE(latest.value("url"_L1).toString(), "https://example.invalid/consent"_L1);
    QCOMPARE(latest.value("elicitationId"_L1).toString(), "elicit-1"_L1);
    // requestedSchema is a form mode member, even though it is REQUIRED there.
    QVERIFY(!latest.contains("requestedSchema"_L1));

    // The 2025-06-18 revision knows form elicitation only.
    const auto old = params.toJsonObject(QtMcp::ProtocolVersion::v2025_06_18);
    QVERIFY(!old.contains("mode"_L1));
    QVERIFY(!old.contains("url"_L1));
    QVERIFY(!old.contains("elicitationId"_L1));
    QVERIFY(old.contains("requestedSchema"_L1));

    // Parsing follows the same rules: without requestedSchema the object is a
    // valid url mode request, but not a valid 2025-06-18 one.
    const QJsonObject object {
        { "mode"_L1, "url"_L1 },
        { "message"_L1, "Sign in to continue"_L1 },
        { "url"_L1, "https://example.invalid/consent"_L1 },
        { "elicitationId"_L1, "elicit-1"_L1 },
    };
    QMcpElicitRequestParams parsed;
    QVERIFY(parsed.fromJsonObject(object));
    QCOMPARE(parsed.mode(), "url"_L1);
    QCOMPARE(parsed.url(), QUrl("https://example.invalid/consent"_L1));
    QCOMPARE(parsed.elicitationId(), "elicit-1"_L1);
    QCOMPARE(parsed.toJsonObject(), object);

    QMcpElicitRequestParams tooOld;
    QVERIFY(!tooOld.fromJsonObject(object, QtMcp::ProtocolVersion::v2025_06_18));

    // A form request keeps working, and mode defaults to "form".
    QMcpElicitRequestParams form;
    QVERIFY(form.fromJsonObject(QJsonObject {
        { "message"_L1, "Confirm?"_L1 },
        { "requestedSchema"_L1, QJsonObject {
            { "type"_L1, "object"_L1 },
            { "properties"_L1, QJsonObject {} },
        }},
    }));
    QCOMPARE(form.mode(), "form"_L1);
    QVERIFY(form.toJsonObject().contains("requestedSchema"_L1));
    // "form" is the default, so it is not serialized.
    QVERIFY(!form.toJsonObject().contains("mode"_L1));
}

QTEST_MAIN(tst_QMcpElicitRequest)
#include "tst_qmcpelicitrequest.moc"
