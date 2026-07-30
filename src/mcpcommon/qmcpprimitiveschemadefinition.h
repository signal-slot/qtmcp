// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPRIMITIVESCHEMADEFINITION_H
#define QMCPPRIMITIVESCHEMADEFINITION_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcpbooleanschema.h>
#include <QtMcpCommon/qmcpenumschema.h>
#include <QtMcpCommon/qmcpnumberschema.h>
#include <QtMcpCommon/qmcpstringschema.h>
#include <QtMcpCommon/qmcptitledmultiselectenumschema.h>
#include <QtMcpCommon/qmcptitledsingleselectenumschema.h>
#include <QtMcpCommon/qmcpuntitledmultiselectenumschema.h>
#include <QtMcpCommon/qmcpuntitledsingleselectenumschema.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPrimitiveSchemaDefinition
    \inmodule QtMcpCommon
    \brief Restricted schema definitions that only allow primitive types without nested objects or arrays.

    Exactly one of the variants is set; refType() tells which one. The 2025-06-18
    revision knows stringSchema, numberSchema, booleanSchema and enumSchema. The
    2025-11-25 revision adds untitledSingleSelectEnumSchema,
    titledSingleSelectEnumSchema, untitledMultiSelectEnumSchema and
    titledMultiSelectEnumSchema, and demotes enumSchema to its
    LegacyTitledEnumSchema.
*/
class Q_MCPCOMMON_EXPORT QMcpPrimitiveSchemaDefinition : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpStringSchema stringSchema READ stringSchema WRITE setStringSchema)
    Q_PROPERTY(QMcpNumberSchema numberSchema READ numberSchema WRITE setNumberSchema)
    Q_PROPERTY(QMcpBooleanSchema booleanSchema READ booleanSchema WRITE setBooleanSchema)
    Q_PROPERTY(QMcpEnumSchema enumSchema READ enumSchema WRITE setEnumSchema)
    Q_PROPERTY(QMcpUntitledSingleSelectEnumSchema untitledSingleSelectEnumSchema READ untitledSingleSelectEnumSchema WRITE setUntitledSingleSelectEnumSchema)
    Q_PROPERTY(QMcpTitledSingleSelectEnumSchema titledSingleSelectEnumSchema READ titledSingleSelectEnumSchema WRITE setTitledSingleSelectEnumSchema)
    Q_PROPERTY(QMcpUntitledMultiSelectEnumSchema untitledMultiSelectEnumSchema READ untitledMultiSelectEnumSchema WRITE setUntitledMultiSelectEnumSchema)
    Q_PROPERTY(QMcpTitledMultiSelectEnumSchema titledMultiSelectEnumSchema READ titledMultiSelectEnumSchema WRITE setTitledMultiSelectEnumSchema)

public:
    QMcpPrimitiveSchemaDefinition() : QMcpAnyOf(new Private) {}
    QMcpPrimitiveSchemaDefinition(const QMcpStringSchema &stringSchema)
        : QMcpAnyOf(new Private) { setStringSchema(stringSchema); }
    QMcpPrimitiveSchemaDefinition(const QMcpNumberSchema &numberSchema)
        : QMcpAnyOf(new Private) { setNumberSchema(numberSchema); }
    QMcpPrimitiveSchemaDefinition(const QMcpBooleanSchema &booleanSchema)
        : QMcpAnyOf(new Private) { setBooleanSchema(booleanSchema); }
    QMcpPrimitiveSchemaDefinition(const QMcpEnumSchema &enumSchema)
        : QMcpAnyOf(new Private) { setEnumSchema(enumSchema); }
    QMcpPrimitiveSchemaDefinition(const QMcpUntitledSingleSelectEnumSchema &untitledSingleSelectEnumSchema)
        : QMcpAnyOf(new Private) { setUntitledSingleSelectEnumSchema(untitledSingleSelectEnumSchema); }
    QMcpPrimitiveSchemaDefinition(const QMcpTitledSingleSelectEnumSchema &titledSingleSelectEnumSchema)
        : QMcpAnyOf(new Private) { setTitledSingleSelectEnumSchema(titledSingleSelectEnumSchema); }
    QMcpPrimitiveSchemaDefinition(const QMcpUntitledMultiSelectEnumSchema &untitledMultiSelectEnumSchema)
        : QMcpAnyOf(new Private) { setUntitledMultiSelectEnumSchema(untitledMultiSelectEnumSchema); }
    QMcpPrimitiveSchemaDefinition(const QMcpTitledMultiSelectEnumSchema &titledMultiSelectEnumSchema)
        : QMcpAnyOf(new Private) { setTitledMultiSelectEnumSchema(titledMultiSelectEnumSchema); }

    QMcpStringSchema stringSchema() const {
        return d<Private>()->stringSchema;
    }

    void setStringSchema(const QMcpStringSchema &stringSchema) {
        // Unlike the other unions the refType has to be set even when the
        // value does not change: a default constructed schema is a perfectly
        // valid variant, e.g. {"type": "string"}.
        setRefType("stringSchema"_ba);
        if (this->stringSchema() == stringSchema) return;
        d<Private>()->stringSchema = stringSchema;
    }

    QMcpNumberSchema numberSchema() const {
        return d<Private>()->numberSchema;
    }

    void setNumberSchema(const QMcpNumberSchema &numberSchema) {
        setRefType("numberSchema"_ba);
        if (this->numberSchema() == numberSchema) return;
        d<Private>()->numberSchema = numberSchema;
    }

    QMcpBooleanSchema booleanSchema() const {
        return d<Private>()->booleanSchema;
    }

    void setBooleanSchema(const QMcpBooleanSchema &booleanSchema) {
        setRefType("booleanSchema"_ba);
        if (this->booleanSchema() == booleanSchema) return;
        d<Private>()->booleanSchema = booleanSchema;
    }

    QMcpEnumSchema enumSchema() const {
        return d<Private>()->enumSchema;
    }

    void setEnumSchema(const QMcpEnumSchema &enumSchema) {
        setRefType("enumSchema"_ba);
        if (this->enumSchema() == enumSchema) return;
        d<Private>()->enumSchema = enumSchema;
    }

    QMcpUntitledSingleSelectEnumSchema untitledSingleSelectEnumSchema() const {
        return d<Private>()->untitledSingleSelectEnumSchema;
    }

    void setUntitledSingleSelectEnumSchema(const QMcpUntitledSingleSelectEnumSchema &untitledSingleSelectEnumSchema) {
        setRefType("untitledSingleSelectEnumSchema"_ba);
        if (this->untitledSingleSelectEnumSchema() == untitledSingleSelectEnumSchema) return;
        d<Private>()->untitledSingleSelectEnumSchema = untitledSingleSelectEnumSchema;
    }

    QMcpTitledSingleSelectEnumSchema titledSingleSelectEnumSchema() const {
        return d<Private>()->titledSingleSelectEnumSchema;
    }

    void setTitledSingleSelectEnumSchema(const QMcpTitledSingleSelectEnumSchema &titledSingleSelectEnumSchema) {
        setRefType("titledSingleSelectEnumSchema"_ba);
        if (this->titledSingleSelectEnumSchema() == titledSingleSelectEnumSchema) return;
        d<Private>()->titledSingleSelectEnumSchema = titledSingleSelectEnumSchema;
    }

    QMcpUntitledMultiSelectEnumSchema untitledMultiSelectEnumSchema() const {
        return d<Private>()->untitledMultiSelectEnumSchema;
    }

    void setUntitledMultiSelectEnumSchema(const QMcpUntitledMultiSelectEnumSchema &untitledMultiSelectEnumSchema) {
        setRefType("untitledMultiSelectEnumSchema"_ba);
        if (this->untitledMultiSelectEnumSchema() == untitledMultiSelectEnumSchema) return;
        d<Private>()->untitledMultiSelectEnumSchema = untitledMultiSelectEnumSchema;
    }

    QMcpTitledMultiSelectEnumSchema titledMultiSelectEnumSchema() const {
        return d<Private>()->titledMultiSelectEnumSchema;
    }

    void setTitledMultiSelectEnumSchema(const QMcpTitledMultiSelectEnumSchema &titledMultiSelectEnumSchema) {
        setRefType("titledMultiSelectEnumSchema"_ba);
        if (this->titledMultiSelectEnumSchema() == titledMultiSelectEnumSchema) return;
        d<Private>()->titledMultiSelectEnumSchema = titledMultiSelectEnumSchema;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    /*!
        Selects the variant by the "type" member of \a object and lets that
        variant parse \a object for the given \a protocolVersion.

        The generic QMcpAnyOf implementation cannot be used here: it picks the
        variant by comparing the JSON keys with the property names, but
        QMcpBooleanSchema and QMcpEnumSchema deliberately name their "default"
        and "enum" members differently because those are C++ keywords. Only the
        variants themselves know how to translate those keys.
    */
    bool fromJsonObject(const QJsonObject &object, QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest) override {
        const auto type = object.value("type"_L1).toString();
        if (type == "boolean"_L1) {
            QMcpBooleanSchema schema;
            if (!schema.fromJsonObject(object, protocolVersion))
                return false;
            setBooleanSchema(schema);
            return true;
        }
        if (type == "integer"_L1 || type == "number"_L1) {
            QMcpNumberSchema schema;
            if (!schema.fromJsonObject(object, protocolVersion))
                return false;
            setNumberSchema(schema);
            return true;
        }
        if (type == "string"_L1) {
            // Four variants use "string" as type. They are told apart by the
            // member that enumerates the accepted values:
            //   "oneOf"              -> TitledSingleSelectEnumSchema
            //   "enum" + "enumNames" -> LegacyTitledEnumSchema (QMcpEnumSchema)
            //   "enum"               -> UntitledSingleSelectEnumSchema
            //   neither              -> StringSchema
            // A plain "enum" without "enumNames" matches both the legacy and
            // the untitled variant, so the newer one wins - but only from
            // 2025-11-25 on, where it exists at all.
            const bool hasSelectVariants = protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25;
            if (hasSelectVariants && object.contains("oneOf"_L1)) {
                QMcpTitledSingleSelectEnumSchema schema;
                if (!schema.fromJsonObject(object, protocolVersion))
                    return false;
                setTitledSingleSelectEnumSchema(schema);
            } else if (object.contains("enum"_L1)) {
                if (!hasSelectVariants || object.contains("enumNames"_L1)) {
                    QMcpEnumSchema schema;
                    if (!schema.fromJsonObject(object, protocolVersion))
                        return false;
                    setEnumSchema(schema);
                } else {
                    QMcpUntitledSingleSelectEnumSchema schema;
                    if (!schema.fromJsonObject(object, protocolVersion))
                        return false;
                    setUntitledSingleSelectEnumSchema(schema);
                }
            } else {
                QMcpStringSchema schema;
                if (!schema.fromJsonObject(object, protocolVersion))
                    return false;
                setStringSchema(schema);
            }
            return true;
        }
        if (type == "array"_L1 && protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25) {
            // Both multi select variants use "array" as type and both require
            // "items"; what that object holds tells them apart: a list of
            // {const, title} options in "anyOf" is the titled variant, a plain
            // list of values in "enum" the untitled one.
            const auto items = object.value("items"_L1).toObject();
            if (items.contains("anyOf"_L1)) {
                QMcpTitledMultiSelectEnumSchema schema;
                if (!schema.fromJsonObject(object, protocolVersion))
                    return false;
                setTitledMultiSelectEnumSchema(schema);
                return true;
            }
            if (items.contains("enum"_L1)) {
                QMcpUntitledMultiSelectEnumSchema schema;
                if (!schema.fromJsonObject(object, protocolVersion))
                    return false;
                setUntitledMultiSelectEnumSchema(schema);
                return true;
            }
            qWarning() << "Multi select enum schema without enumerated items" << items;
            return false;
        }
        qWarning() << "Unknown primitive schema type" << type;
        return false;
    }

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpStringSchema stringSchema;
        QMcpNumberSchema numberSchema;
        QMcpBooleanSchema booleanSchema;
        QMcpEnumSchema enumSchema;
        QMcpUntitledSingleSelectEnumSchema untitledSingleSelectEnumSchema;
        QMcpTitledSingleSelectEnumSchema titledSingleSelectEnumSchema;
        QMcpUntitledMultiSelectEnumSchema untitledMultiSelectEnumSchema;
        QMcpTitledMultiSelectEnumSchema titledMultiSelectEnumSchema;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPrimitiveSchemaDefinition)

QT_END_NAMESPACE

#endif // QMCPPRIMITIVESCHEMADEFINITION_H
