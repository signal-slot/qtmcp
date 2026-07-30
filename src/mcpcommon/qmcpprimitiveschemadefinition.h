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

QT_BEGIN_NAMESPACE

/*! \class QMcpPrimitiveSchemaDefinition
    \inmodule QtMcpCommon
    \brief Restricted schema definitions that only allow primitive types without nested objects or arrays.

    Exactly one of stringSchema, numberSchema, booleanSchema and enumSchema is
    set; refType() tells which one.
*/
class Q_MCPCOMMON_EXPORT QMcpPrimitiveSchemaDefinition : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpStringSchema stringSchema READ stringSchema WRITE setStringSchema)
    Q_PROPERTY(QMcpNumberSchema numberSchema READ numberSchema WRITE setNumberSchema)
    Q_PROPERTY(QMcpBooleanSchema booleanSchema READ booleanSchema WRITE setBooleanSchema)
    Q_PROPERTY(QMcpEnumSchema enumSchema READ enumSchema WRITE setEnumSchema)

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
            // Both QMcpStringSchema and QMcpEnumSchema use "string" as type;
            // only the latter enumerates the accepted values.
            if (object.contains("enum"_L1)) {
                QMcpEnumSchema schema;
                if (!schema.fromJsonObject(object, protocolVersion))
                    return false;
                setEnumSchema(schema);
            } else {
                QMcpStringSchema schema;
                if (!schema.fromJsonObject(object, protocolVersion))
                    return false;
                setStringSchema(schema);
            }
            return true;
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

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPrimitiveSchemaDefinition)

QT_END_NAMESPACE

#endif // QMCPPRIMITIVESCHEMADEFINITION_H
