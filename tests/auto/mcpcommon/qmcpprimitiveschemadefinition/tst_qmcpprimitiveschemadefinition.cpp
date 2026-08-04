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
    // Parses \a json into \a schema for \a protocolVersion. Returns false when
    // the JSON is malformed or not accepted by the schema.
    static bool parse(QMcpPrimitiveSchemaDefinition *schema, const QByteArray &json,
                      QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest);

private slots:
    void defaultValues();
    void roundTrip_data();
    void roundTrip();
    void booleanDefaultKey();
    void enumKey();
    void integerType();
    void stringWithoutEnumIsStringSchema();
    void stringWithEnumIsSingleSelect();
    void enumNamesIsLegacyEnumSchema();
    void selectVariantKeys();
    void selectVariantsNeed20251125();
    void multiSelectWithoutEnumeratedItems();
    void unknownType();
    void copy();
};

bool tst_QMcpPrimitiveSchemaDefinition::parse(QMcpPrimitiveSchemaDefinition *schema,
                                              const QByteArray &json,
                                              QtMcp::ProtocolVersion protocolVersion)
{
    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return false;
    return schema->fromJsonObject(doc.object(), protocolVersion);
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

    // enumNames marks the legacy variant, which 2025-11-25 keeps for
    // compatibility.
    QTest::newRow("enumSchema") << R"({
        "type": "string",
        "title": "Favorite color",
        "description": "Pick one",
        "enum": ["red", "green", "blue"],
        "enumNames": ["Red", "Green", "Blue"],
        "default": "red"
    })"_ba << "enumSchema"_ba;

    QTest::newRow("untitledSingleSelectEnumSchema") << R"({
        "type": "string",
        "title": "Favorite color",
        "description": "Pick one",
        "enum": ["red", "green", "blue"],
        "default": "red"
    })"_ba << "untitledSingleSelectEnumSchema"_ba;

    QTest::newRow("titledSingleSelectEnumSchema") << R"({
        "type": "string",
        "title": "Favorite color",
        "oneOf": [
            { "const": "red", "title": "Red" },
            { "const": "green", "title": "Green" }
        ],
        "default": "red"
    })"_ba << "titledSingleSelectEnumSchema"_ba;

    QTest::newRow("untitledMultiSelectEnumSchema") << R"({
        "type": "array",
        "title": "Toppings",
        "items": {
            "type": "string",
            "enum": ["cheese", "basil"]
        },
        "minItems": 1,
        "maxItems": 2,
        "default": ["cheese"]
    })"_ba << "untitledMultiSelectEnumSchema"_ba;

    QTest::newRow("titledMultiSelectEnumSchema") << R"({
        "type": "array",
        "title": "Toppings",
        "items": {
            "anyOf": [
                { "const": "cheese", "title": "Cheese" },
                { "const": "basil", "title": "Basil" }
            ]
        },
        "minItems": 1,
        "default": ["cheese"]
    })"_ba << "titledMultiSelectEnumSchema"_ba;

    // The members 2025-11-25 added to the schemas of 2025-06-18.
    QTest::newRow("stringSchema default") << R"({
        "type": "string",
        "default": "anonymous"
    })"_ba << "stringSchema"_ba;

    QTest::newRow("numberSchema default") << R"({
        "type": "number",
        "default": 0.5
    })"_ba << "numberSchema"_ba;

    // Only the members the protocol requires.
    QTest::newRow("minimal stringSchema") << R"({
        "type": "string"
    })"_ba << "stringSchema"_ba;

    QTest::newRow("minimal untitledSingleSelectEnumSchema") << R"({
        "type": "string",
        "enum": ["yes", "no"]
    })"_ba << "untitledSingleSelectEnumSchema"_ba;

    QTest::newRow("minimal untitledMultiSelectEnumSchema") << R"({
        "type": "array",
        "items": {
            "type": "string",
            "enum": ["yes", "no"]
        }
    })"_ba << "untitledMultiSelectEnumSchema"_ba;

    QTest::newRow("minimal titledMultiSelectEnumSchema") << R"({
        "type": "array",
        "items": {
            "anyOf": [
                { "const": "yes", "title": "Yes" }
            ]
        }
    })"_ba << "titledMultiSelectEnumSchema"_ba;
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

void tst_QMcpPrimitiveSchemaDefinition::stringWithEnumIsSingleSelect()
{
    // A plain "enum" matches both LegacyTitledEnumSchema and
    // UntitledSingleSelectEnumSchema. Since 2025-11-25 the newer one wins.
    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(parse(&schema, R"({"type": "string", "enum": ["a", "b"]})"_ba));
    QCOMPARE(schema.refType(), "untitledSingleSelectEnumSchema"_ba);
    QCOMPARE(schema.untitledSingleSelectEnumSchema().enumValues().size(), 2);
}

void tst_QMcpPrimitiveSchemaDefinition::enumNamesIsLegacyEnumSchema()
{
    // enumNames only exists in the legacy variant, so it decides.
    QMcpPrimitiveSchemaDefinition schema;
    QVERIFY(parse(&schema, R"({
        "type": "string",
        "enum": ["a", "b"],
        "enumNames": ["A", "B"]
    })"_ba));
    QCOMPARE(schema.refType(), "enumSchema"_ba);
    QCOMPARE(schema.enumSchema().enumNames(), QList<QString>({ "A"_L1, "B"_L1 }));
}

void tst_QMcpPrimitiveSchemaDefinition::selectVariantKeys()
{
    // "default", "enum" and "const" are C++ keywords, so the properties of the
    // select variants are named differently. Only the JSON names may appear on
    // the wire.
    QMcpPrimitiveSchemaDefinition untitledSingle;
    QVERIFY(parse(&untitledSingle, R"({
        "type": "string",
        "enum": ["red", "green"],
        "default": "red"
    })"_ba));
    QCOMPARE(untitledSingle.refType(), "untitledSingleSelectEnumSchema"_ba);
    QCOMPARE(untitledSingle.untitledSingleSelectEnumSchema().enumValues(),
             QList<QString>({ "red"_L1, "green"_L1 }));
    QCOMPARE(untitledSingle.untitledSingleSelectEnumSchema().defaultValue(), "red"_L1);
    const auto untitledSingleObject = untitledSingle.toJsonObject();
    QVERIFY(!untitledSingleObject.contains("enumValues"_L1));
    QVERIFY(!untitledSingleObject.contains("defaultValue"_L1));
    QCOMPARE(untitledSingleObject.value("enum"_L1).toArray(),
             QJsonArray({ "red"_L1, "green"_L1 }));
    QCOMPARE(untitledSingleObject.value("default"_L1).toString(), "red"_L1);

    QMcpPrimitiveSchemaDefinition titledSingle;
    QVERIFY(parse(&titledSingle, R"({
        "type": "string",
        "oneOf": [
            { "const": "red", "title": "Red" }
        ],
        "default": "red"
    })"_ba));
    QCOMPARE(titledSingle.refType(), "titledSingleSelectEnumSchema"_ba);
    const auto oneOf = titledSingle.titledSingleSelectEnumSchema().oneOf();
    QCOMPARE(oneOf.size(), 1);
    QCOMPARE(oneOf.at(0).constValue(), "red"_L1);
    QCOMPARE(oneOf.at(0).title(), "Red"_L1);
    const auto titledSingleObject = titledSingle.toJsonObject();
    QVERIFY(!titledSingleObject.contains("defaultValue"_L1));
    QCOMPARE(titledSingleObject.value("default"_L1).toString(), "red"_L1);
    const auto oneOfArray = titledSingleObject.value("oneOf"_L1).toArray();
    QCOMPARE(oneOfArray.size(), 1);
    QVERIFY(!oneOfArray.at(0).toObject().contains("constValue"_L1));
    QCOMPARE(oneOfArray.at(0).toObject().value("const"_L1).toString(), "red"_L1);

    QMcpPrimitiveSchemaDefinition untitledMulti;
    QVERIFY(parse(&untitledMulti, R"({
        "type": "array",
        "items": {
            "type": "string",
            "enum": ["cheese", "basil"]
        },
        "default": ["cheese"]
    })"_ba));
    QCOMPARE(untitledMulti.refType(), "untitledMultiSelectEnumSchema"_ba);
    QCOMPARE(untitledMulti.untitledMultiSelectEnumSchema().items().enumValues(),
             QList<QString>({ "cheese"_L1, "basil"_L1 }));
    QCOMPARE(untitledMulti.untitledMultiSelectEnumSchema().defaultValue(),
             QList<QString>({ "cheese"_L1 }));
    const auto untitledMultiObject = untitledMulti.toJsonObject();
    QVERIFY(!untitledMultiObject.contains("defaultValue"_L1));
    QCOMPARE(untitledMultiObject.value("default"_L1).toArray(), QJsonArray({ "cheese"_L1 }));
    const auto untitledItems = untitledMultiObject.value("items"_L1).toObject();
    QVERIFY(!untitledItems.contains("enumValues"_L1));
    QCOMPARE(untitledItems.value("enum"_L1).toArray(), QJsonArray({ "cheese"_L1, "basil"_L1 }));
    QCOMPARE(untitledItems.value("type"_L1).toString(), "string"_L1);

    QMcpPrimitiveSchemaDefinition titledMulti;
    QVERIFY(parse(&titledMulti, R"({
        "type": "array",
        "items": {
            "anyOf": [
                { "const": "cheese", "title": "Cheese" }
            ]
        },
        "default": ["cheese"]
    })"_ba));
    QCOMPARE(titledMulti.refType(), "titledMultiSelectEnumSchema"_ba);
    const auto anyOf = titledMulti.titledMultiSelectEnumSchema().items().anyOf();
    QCOMPARE(anyOf.size(), 1);
    QCOMPARE(anyOf.at(0).constValue(), "cheese"_L1);
    QCOMPARE(anyOf.at(0).title(), "Cheese"_L1);
    const auto titledMultiObject = titledMulti.toJsonObject();
    QVERIFY(!titledMultiObject.contains("defaultValue"_L1));
    QCOMPARE(titledMultiObject.value("default"_L1).toArray(), QJsonArray({ "cheese"_L1 }));
    const auto anyOfArray = titledMultiObject.value("items"_L1).toObject().value("anyOf"_L1).toArray();
    QCOMPARE(anyOfArray.size(), 1);
    QVERIFY(!anyOfArray.at(0).toObject().contains("constValue"_L1));
    QCOMPARE(anyOfArray.at(0).toObject().value("const"_L1).toString(), "cheese"_L1);
}

void tst_QMcpPrimitiveSchemaDefinition::selectVariantsNeed20251125()
{
    // The select variants were introduced in 2025-11-25. Older revisions know
    // one enumeration schema only, and no array schema at all.
    QMcpPrimitiveSchemaDefinition enumSchema;
    QVERIFY(parse(&enumSchema, R"({"type": "string", "enum": ["a", "b"]})"_ba,
                  QtMcp::ProtocolVersion::v2025_06_18));
    QCOMPARE(enumSchema.refType(), "enumSchema"_ba);
    // "default" is not part of the 2025-06-18 schemas either.
    QMcpEnumSchema legacy;
    legacy.setEnumValues({ "a"_L1, "b"_L1 });
    legacy.setDefaultValue("a"_L1);
    const QMcpPrimitiveSchemaDefinition withDefault(legacy);
    QVERIFY(!withDefault.toJsonObject(QtMcp::ProtocolVersion::v2025_06_18).contains("default"_L1));
    QVERIFY(withDefault.toJsonObject().contains("default"_L1));

    QMcpPrimitiveSchemaDefinition multiSelect;
    QTest::ignoreMessage(QtWarningMsg, "Unknown primitive schema type \"array\"");
    QVERIFY(!parse(&multiSelect, R"({
        "type": "array",
        "items": { "type": "string", "enum": ["a"] }
    })"_ba, QtMcp::ProtocolVersion::v2025_06_18));
    QVERIFY(multiSelect.isNull());
}

void tst_QMcpPrimitiveSchemaDefinition::multiSelectWithoutEnumeratedItems()
{
    // "items" is required and has to enumerate the options, either as "enum" or
    // as "anyOf"; there is nothing to fall back to.
    QMcpPrimitiveSchemaDefinition schema;
    QTest::ignoreMessage(QtWarningMsg,
                         "Multi select enum schema without enumerated items QJsonObject()");
    QVERIFY(!schema.fromJsonObject(QJsonObject { { "type"_L1, "array"_L1 } }));
    QVERIFY(schema.isNull());
}

void tst_QMcpPrimitiveSchemaDefinition::unknownType()
{
    QMcpPrimitiveSchemaDefinition schema;
    QTest::ignoreMessage(QtWarningMsg, "Unknown primitive schema type \"object\"");
    QVERIFY(!schema.fromJsonObject(QJsonObject { { "type"_L1, "object"_L1 } }));
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
