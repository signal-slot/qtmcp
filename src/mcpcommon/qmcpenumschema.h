// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPENUMSCHEMA_H
#define QMCPENUMSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpEnumSchema
    \inmodule QtMcpCommon
    \brief A restricted JSON Schema for a value picked from a fixed set of strings.

    This type is the LegacyTitledEnumSchema of the 2025-11-25 revision, which
    that revision deprecates in favour of QMcpTitledSingleSelectEnumSchema
    because enumNames is not part of JSON Schema 2020-12. It is still the only
    enumeration schema of the 2025-06-18 revision, so it is kept as is.

    \note The schema calls the properties holding the accepted values and the
    initial value "enum" and "default", which are C++ keywords and therefore
    cannot be used as Q_PROPERTY names. The properties are named enumValues and
    defaultValue instead and the JSON keys are translated by fromJsonObject()
    and toJsonObject().

    \sa QMcpPrimitiveSchemaDefinition, QMcpTitledSingleSelectEnumSchema
*/
class Q_MCPCOMMON_EXPORT QMcpEnumSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpEnumSchema::defaultValue
        \brief The value to be used unless the user picks another one.

        This property is serialized as "default".
        \since MCP 2025-11-25
    */
    Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue)

    /*!
        \property QMcpEnumSchema::description
        \brief A human-readable description of the requested value.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpEnumSchema::enumNames
        \brief Human-readable labels for enumValues, in the same order.
    */
    Q_PROPERTY(QList<QString> enumNames READ enumNames WRITE setEnumNames)

    /*!
        \property QMcpEnumSchema::enumValues
        \brief The accepted values.

        This property is serialized as "enum".
    */
    Q_PROPERTY(QList<QString> enumValues READ enumValues WRITE setEnumValues REQUIRED)

    /*!
        \property QMcpEnumSchema::title
        \brief A human-readable title of the requested value.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpEnumSchema() : QMcpGadget(new Private) {}

    QString defaultValue() const {
        return d<Private>()->defaultValue;
    }

    void setDefaultValue(const QString &defaultValue) {
        if (this->defaultValue() == defaultValue) return;
        d<Private>()->defaultValue = defaultValue;
    }

    QString description() const {
        return d<Private>()->description;
    }

    void setDescription(const QString &description) {
        if (this->description() == description) return;
        d<Private>()->description = description;
    }

    QList<QString> enumNames() const {
        return d<Private>()->enumNames;
    }

    void setEnumNames(const QList<QString> &enumNames) {
        if (this->enumNames() == enumNames) return;
        d<Private>()->enumNames = enumNames;
    }

    QList<QString> enumValues() const {
        return d<Private>()->enumValues;
    }

    void setEnumValues(const QList<QString> &enumValues) {
        if (this->enumValues() == enumValues) return;
        d<Private>()->enumValues = enumValues;
    }

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
    }

    static QByteArray type() { return QByteArrayLiteral("string"); }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    bool fromJsonObject(const QJsonObject &object, QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest) override {
        auto renamed = renamedKey(object, enumJsonKey(), enumPropertyKey());
        renamed = renamedKey(renamed, defaultJsonKey(), defaultPropertyKey());
        return QMcpGadget::fromJsonObject(renamed, protocolVersion);
    }

    QJsonObject toJsonObject(QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest) const override {
        auto object = renamedKey(QMcpGadget::toJsonObject(protocolVersion), enumPropertyKey(), enumJsonKey());
        return renamedKey(object, defaultPropertyKey(), defaultJsonKey());
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "defaultValue")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    static QString enumJsonKey() { return QStringLiteral("enum"); }
    static QString enumPropertyKey() { return QStringLiteral("enumValues"); }
    static QString defaultJsonKey() { return QStringLiteral("default"); }
    static QString defaultPropertyKey() { return QStringLiteral("defaultValue"); }

    struct Private : public QMcpGadget::Private {
        QString defaultValue;
        QString description;
        QList<QString> enumNames;
        QList<QString> enumValues;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpEnumSchema)

QT_END_NAMESPACE

#endif // QMCPENUMSCHEMA_H
