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

QTEST_MAIN(tst_QMcpElicitRequest)
#include "tst_qmcpelicitrequest.moc"
