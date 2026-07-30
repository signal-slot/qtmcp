// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/qmcpprimitiveschemadefinition.h>
#include <QtTest/QTest>

class tst_QMcpPrimitiveSchemaDefinition : public QObject
{
    Q_OBJECT

private:
    // Parses \a json into \a schema. Returns false when the JSON is malformed
    // or not accepted by the schema.
    static bool parse(QMcpPrimitiveSchemaDefinition *schema, const QByteArray &json);

private slots:
    void defaultValues();
    void roundTrip_data();
    void roundTrip();
    void booleanDefaultKey();
    void enumKey();
    void integerType();
    void stringWithoutEnumIsStringSchema();
    void stringWithEnumIsEnumSchema();
    void unknownType();
    void copy();
};

bool tst_QMcpPrimitiveSchemaDefinition::parse(QMcpPrimitiveSchemaDefinition *schema,
                                              const QByteArray &json)
{
    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return false;
    return schema->fromJsonObject(doc.object());
}

void tst_QMcpPrimitiveSchemaDefinition::defaultValues()
{
    const QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(schema.isNull());
    QVERIFY(schema.refType().isEmpty());
}

void tst_QMcpPrimitiveSchemaDefinition::roundTrip_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QByteArray>("refType");

    QTest::newRow("booleanSchema") << R"({
        "type": "boolean",
        "title": "Accept the terms",
        "description": "Whether the user accepts the terms",
        "default": true
    })"_ba << "booleanSchema"_ba;

    QTest::newRow("stringSchema") << R"({
        "type": "string",
        "title": "Nickname",
        "description": "How we should address the user",
        "format": "email",
        "minLength": 1,
        "maxLength": 32
    })"_ba << "stringSchema"_ba;

    QTest::newRow("numberSchema") << R"({
        "type": "number",
        "title": "Ratio",
        "description": "A value between 0 and 1",
        "minimum": 0.25,
        "maximum": 0.75
    })"_ba << "numberSchema"_ba;

    QTest::newRow("numberSchema integer") << R"({
        "type": "integer",
        "title": "Age",
        "minimum": 1,
        "maximum": 150
    })"_ba << "numberSchema"_ba;

    QTest::newRow("enumSchema") << R"({
        "type": "string",
        "title": "Favorite color",
        "description": "Pick one",
        "enum": ["red", "green", "blue"],
        "enumNames": ["Red", "Green", "Blue"]
    })"_ba << "enumSchema"_ba;

    // Only the members the protocol requires.
    QTest::newRow("minimal stringSchema") << R"({
        "type": "string"
    })"_ba << "stringSchema"_ba;

    QTest::newRow("minimal enumSchema") << R"({
        "type": "string",
        "enum": ["yes", "no"]
    })"_ba << "enumSchema"_ba;
}

void tst_QMcpPrimitiveSchemaDefinition::roundTrip()
{
    QFETCH(QByteArray, json);
    QFETCH(QByteArray, refType);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());
    const auto object = doc.object();

    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(schema.fromJsonObject(object));

    // The variant is picked by the "type" member (and by the presence of
    // "enum" for strings).
    QCOMPARE(schema.refType(), refType);
    QVERIFY(!schema.isNull());

    // Every member of the input has to come back unchanged, including the
    // members whose JSON key differs from the property name.
    QCOMPARE(schema.toJsonObject(), object);
}

void tst_QMcpPrimitiveSchemaDefinition::booleanDefaultKey()
{
    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(parse(&schema, R"({"type": "boolean", "default": true})"_ba));
    QCOMPARE(schema.refType(), "booleanSchema"_ba);
    // The JSON key is "default", the C++ property is defaultValue.
    QCOMPARE(schema.booleanSchema().defaultValue(), true);

    const auto object = schema.toJsonObject();
    QVERIFY(object.contains("default"_L1));
    QVERIFY(!object.contains("defaultValue"_L1));
    QCOMPARE(object.value("default"_L1).toBool(), true);

    // false is the default of the property, so it is not serialized at all.
    QMcpBooleanSchema booleanSchema;
    booleanSchema.setDefaultValue(false);
    const QMcpPrimitiveSchemaDefinition falseSchema(booleanSchema);
    const auto falseObject = falseSchema.toJsonObject();
    QVERIFY(!falseObject.contains("default"_L1));
    QVERIFY(!falseObject.contains("defaultValue"_L1));
    QCOMPARE(falseObject.value("type"_L1).toString(), "boolean"_L1);
}

void tst_QMcpPrimitiveSchemaDefinition::enumKey()
{
    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(parse(&schema, R"({
        "type": "string",
        "enum": ["red", "green"],
        "enumNames": ["Red", "Green"]
    })"_ba));
    QCOMPARE(schema.refType(), "enumSchema"_ba);
    // The JSON key is "enum", the C++ property is enumValues.
    QCOMPARE(schema.enumSchema().enumValues(), QList<QString>({ "red"_L1, "green"_L1 }));
    QCOMPARE(schema.enumSchema().enumNames(), QList<QString>({ "Red"_L1, "Green"_L1 }));

    const auto object = schema.toJsonObject();
    QVERIFY(object.contains("enum"_L1));
    QVERIFY(!object.contains("enumValues"_L1));
    QCOMPARE(object.value("enum"_L1).toArray(), QJsonArray({ "red"_L1, "green"_L1 }));
    // enumNames keeps its name, it is not a C++ keyword.
    QCOMPARE(object.value("enumNames"_L1).toArray(), QJsonArray({ "Red"_L1, "Green"_L1 }));
}

void tst_QMcpPrimitiveSchemaDefinition::integerType()
{
    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(parse(&schema, R"({"type": "integer", "minimum": 1, "maximum": 10})"_ba));
    // "integer" and "number" share QMcpNumberSchema...
    QCOMPARE(schema.refType(), "numberSchema"_ba);
    QCOMPARE(schema.numberSchema().type(), "integer"_L1);
    // ...but the distinction must not be lost on the way out.
    QCOMPARE(schema.toJsonObject().value("type"_L1).toString(), "integer"_L1);

    QMcpPrimitiveSchemaDefinition numberSchema;
    QVERIFY(parse(&numberSchema, R"({"type": "number", "minimum": 1})"_ba));
    QCOMPARE(numberSchema.refType(), "numberSchema"_ba);
    QCOMPARE(numberSchema.numberSchema().type(), "number"_L1);
    QCOMPARE(numberSchema.toJsonObject().value("type"_L1).toString(), "number"_L1);
}

void tst_QMcpPrimitiveSchemaDefinition::stringWithoutEnumIsStringSchema()
{
    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(parse(&schema, R"({"type": "string", "minLength": 2, "maxLength": 8})"_ba));
    QCOMPARE(schema.refType(), "stringSchema"_ba);
    QCOMPARE(schema.stringSchema().minLength(), 2);
    QCOMPARE(schema.stringSchema().maxLength(), 8);
    QVERIFY(schema.enumSchema().enumValues().isEmpty());
}

void tst_QMcpPrimitiveSchemaDefinition::stringWithEnumIsEnumSchema()
{
    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(parse(&schema, R"({"type": "string", "enum": ["a", "b"]})"_ba));
    QCOMPARE(schema.refType(), "enumSchema"_ba);
    QCOMPARE(schema.enumSchema().enumValues().size(), 2);
}

void tst_QMcpPrimitiveSchemaDefinition::unknownType()
{
    QMcpPrimitiveSchemaDefinition schema;
    QTest::ignoreMessage(QtWarningMsg, "Unknown primitive schema type \"array\"");
    QVERIFY(!schema.fromJsonObject(QJsonObject { { "type"_L1, "array"_L1 } }));
    QVERIFY(schema.isNull());
}

void tst_QMcpPrimitiveSchemaDefinition::copy()
{
    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(parse(&schema, R"({"type": "boolean", "title": "Agree", "default": true})"_ba));
    const auto expected = schema.toJsonObject();

    QMcpPrimitiveSchemaDefinition copyConstructed(schema);
    QCOMPARE(copyConstructed.refType(), "booleanSchema"_ba);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpPrimitiveSchemaDefinition assigned;
    assigned = copyConstructed;
    QCOMPARE(assigned.refType(), "booleanSchema"_ba);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Turning the original into another variant must not affect the copies.
    QMcpStringSchema stringSchema;
    stringSchema.setTitle("Nickname"_L1);
    schema.setStringSchema(stringSchema);
    QCOMPARE(schema.refType(), "stringSchema"_ba);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

QTEST_MAIN(tst_QMcpPrimitiveSchemaDefinition)
#include "tst_qmcpprimitiveschemadefinition.moc"
