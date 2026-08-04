// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPBOOLEANSCHEMA_H
#define QMCPBOOLEANSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpBooleanSchema
    \inmodule QtMcpCommon
    \brief A restricted JSON Schema for a boolean value requested via elicitation.

    \note The schema calls the property holding the initial value "default",
    which is a C++ keyword and therefore cannot be used as a Q_PROPERTY name.
    The property is named defaultValue instead and the JSON key is translated
    by fromJsonObject() and toJsonObject().

    \sa QMcpPrimitiveSchemaDefinition
*/
class Q_MCPCOMMON_EXPORT QMcpBooleanSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpBooleanSchema::defaultValue
        \brief The value to be used unless the user picks another one.

        This property is serialized as "default". The default value is false.
    */
    Q_PROPERTY(bool defaultValue READ defaultValue WRITE setDefaultValue)

    /*!
        \property QMcpBooleanSchema::description
        \brief A human-readable description of the requested value.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpBooleanSchema::title
        \brief A human-readable title of the requested value.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpBooleanSchema() : QMcpGadget(new Private) {}

    bool defaultValue() const {
        return d<Private>()->defaultValue;
    }

    void setDefaultValue(bool defaultValue) {
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

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
    }

    static QByteArray type() { return QByteArrayLiteral("boolean"); }

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
        bool defaultValue = false;
        QString description;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpBooleanSchema)

QT_END_NAMESPACE

#endif // QMCPBOOLEANSCHEMA_H
