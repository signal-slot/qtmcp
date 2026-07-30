// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTITLEDSINGLESELECTENUMSCHEMA_H
#define QMCPTITLEDSINGLESELECTENUMSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcptitledsingleselectenumschemaoneof.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTitledSingleSelectEnumSchema
    \inmodule QtMcpCommon
    \brief A restricted JSON Schema for one value picked from a fixed set of options, each with a display title.

    The user picks exactly one of oneOf. This is the replacement for the
    deprecated QMcpEnumSchema (LegacyTitledEnumSchema): instead of two parallel
    lists it keeps every value together with its label, which JSON Schema
    2020-12 allows.

    \note The schema calls the property holding the initial value "default",
    which is a C++ keyword and therefore cannot be used as a Q_PROPERTY name.
    The property is named defaultValue instead and the JSON key is translated
    by fromJsonObject() and toJsonObject().

    \sa QMcpPrimitiveSchemaDefinition, QMcpEnumSchema
    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpTitledSingleSelectEnumSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpTitledSingleSelectEnumSchema::defaultValue
        \brief The value to be used unless the user picks another one.

        This property is serialized as "default". It holds the constValue of one
        of the oneOf options, not its title.
    */
    Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue)

    /*!
        \property QMcpTitledSingleSelectEnumSchema::description
        \brief A human-readable description of the requested value.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpTitledSingleSelectEnumSchema::oneOf
        \brief The options to choose from, each with a value and a display label.
    */
    Q_PROPERTY(QList<QMcpTitledSingleSelectEnumSchemaOneOf> oneOf READ oneOf WRITE setOneOf REQUIRED)

    /*!
        \property QMcpTitledSingleSelectEnumSchema::title
        \brief A human-readable title of the requested value.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpTitledSingleSelectEnumSchema() : QMcpGadget(new Private) {
        // QMcpGadget looks the element type of a list property up by name.
        qRegisterMetaType<QMcpTitledSingleSelectEnumSchemaOneOf>();
    }

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

    QList<QMcpTitledSingleSelectEnumSchemaOneOf> oneOf() const {
        return d<Private>()->oneOf;
    }

    void setOneOf(const QList<QMcpTitledSingleSelectEnumSchemaOneOf> &oneOf) {
        if (this->oneOf() == oneOf) return;
        d<Private>()->oneOf = oneOf;
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
        return QMcpGadget::fromJsonObject(renamedKey(object, jsonKey(), propertyKey()), protocolVersion);
    }

    QJsonObject toJsonObject(QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest) const override {
        return renamedKey(QMcpGadget::toJsonObject(protocolVersion), propertyKey(), jsonKey());
    }

private:
    static QString jsonKey() { return QStringLiteral("default"); }
    static QString propertyKey() { return QStringLiteral("defaultValue"); }

    struct Private : public QMcpGadget::Private {
        QString defaultValue;
        QString description;
        QList<QMcpTitledSingleSelectEnumSchemaOneOf> oneOf;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTitledSingleSelectEnumSchema)

QT_END_NAMESPACE

#endif // QMCPTITLEDSINGLESELECTENUMSCHEMA_H
