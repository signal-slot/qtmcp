// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpToolInputSchema>
#include <QtTest/QTest>

class tst_QMcpToolInputSchema : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpToolInputSchema::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Basic schema
    QTest::newRow("basic") << R"({
        "type": "object",
        "properties": {
            "type": "string"
        }
    })"_ba
    << QVariantMap {
        { "type", "object"_ba },
        { "properties", QVariantMap {
            { "type", "string" }
        }}
    };

    // Schema with required properties
    QTest::newRow("with required") << R"({
        "type": "object",
        "properties": {
            "name": {
                "type": "string"
            },
            "age": {
                "type": "number"
            }
        },
        "required": ["name", "age"]
    })"_ba
    << QVariantMap {
        { "type", "object"_ba },
        { "properties", QVariantMap {
            { "name", QVariantMap {
                { "type", "string" }
            }},
            { "age", QVariantMap {
                { "type", "number" }
            }}
        }},
        { "required", QStringList { "name", "age" } }
    };

    // Schema with empty properties
    QTest::newRow("empty properties") << R"({
        "type": "object"
    })"_ba
    << QVariantMap {
        { "type", "object"_ba }
    };

    // Schema with empty required list
    QTest::newRow("empty required") << R"({
        "type": "object",
        "properties": {
            "type": "string"
        },
        "required": []
    })"_ba
    << QVariantMap {
        { "type", "object"_ba },
        { "properties", QVariantMap {
            { "type", "string" }
        }}
    };
}

void tst_QMcpToolInputSchema::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpToolInputSchema schema;
    QVERIFY(schema.fromJsonObject(object));
    TestHelper::verify(&schema, data);

    // Verify conversion
    const auto converted = schema.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpToolInputSchema::copy_data()
{
    convert_data();
}

void tst_QMcpToolInputSchema::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpToolInputSchema schema;
    QVERIFY(schema.fromJsonObject(object));

    // Test copy constructor
    QMcpToolInputSchema schema2(schema);
    QCOMPARE(schema2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpToolInputSchema schema3;
    schema3 = schema2;
    QCOMPARE(schema3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpToolInputSchema)
#include "tst_qmcptoolinputschema.moc"
