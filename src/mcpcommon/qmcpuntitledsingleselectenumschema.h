// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPUNTITLEDSINGLESELECTENUMSCHEMA_H
#define QMCPUNTITLEDSINGLESELECTENUMSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpUntitledSingleSelectEnumSchema
    \inmodule QtMcpCommon
    \brief A restricted JSON Schema for one value picked from a fixed set of strings, without display titles.

    The user picks exactly one of enumValues. Use
    QMcpTitledSingleSelectEnumSchema when the options need a display label.

    \note The schema calls the properties holding the accepted values and the
    initial value "enum" and "default", which are C++ keywords and therefore
    cannot be used as Q_PROPERTY names. The properties are named enumValues and
    defaultValue instead and the JSON keys are translated by fromJsonObject()
    and toJsonObject().

    \sa QMcpPrimitiveSchemaDefinition
    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpUntitledSingleSelectEnumSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpUntitledSingleSelectEnumSchema::defaultValue
        \brief The value to be used unless the user picks another one.

        This property is serialized as "default".
    */
    Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue)

    /*!
        \property QMcpUntitledSingleSelectEnumSchema::description
        \brief A human-readable description of the requested value.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpUntitledSingleSelectEnumSchema::enumValues
        \brief The values to choose from.

        This property is serialized as "enum".
    */
    Q_PROPERTY(QList<QString> enumValues READ enumValues WRITE setEnumValues REQUIRED)

    /*!
        \property QMcpUntitledSingleSelectEnumSchema::title
        \brief A human-readable title of the requested value.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpUntitledSingleSelectEnumSchema() : QMcpGadget(new Private) {}

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

private:
    static QString enumJsonKey() { return QStringLiteral("enum"); }
    static QString enumPropertyKey() { return QStringLiteral("enumValues"); }
    static QString defaultJsonKey() { return QStringLiteral("default"); }
    static QString defaultPropertyKey() { return QStringLiteral("defaultValue"); }

    struct Private : public QMcpGadget::Private {
        QString defaultValue;
        QString description;
        QList<QString> enumValues;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpUntitledSingleSelectEnumSchema)

QT_END_NAMESPACE

#endif // QMCPUNTITLEDSINGLESELECTENUMSCHEMA_H
