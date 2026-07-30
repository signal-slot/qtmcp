// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSTRINGSCHEMA_H
#define QMCPSTRINGSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpStringSchema
    \inmodule QtMcpCommon
    \brief A restricted JSON Schema for a string value requested via elicitation.

    \note The schema calls the property holding the initial value "default",
    which is a C++ keyword and therefore cannot be used as a Q_PROPERTY name.
    The property is named defaultValue instead and the JSON key is translated
    by fromJsonObject() and toJsonObject().

    \sa QMcpPrimitiveSchemaDefinition
*/
class Q_MCPCOMMON_EXPORT QMcpStringSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpStringSchema::defaultValue
        \brief The value to be used unless the user enters another one.

        This property is serialized as "default".
        \since MCP 2025-11-25
    */
    Q_PROPERTY(QString defaultValue READ defaultValue WRITE setDefaultValue)

    /*!
        \property QMcpStringSchema::description
        \brief A human-readable description of the requested value.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpStringSchema::format
        \brief The expected format of the value.

        One of "date", "date-time", "email" or "uri". Empty if unconstrained.
    */
    Q_PROPERTY(QString format READ format WRITE setFormat)

    /*!
        \property QMcpStringSchema::maxLength
        \brief The largest accepted number of characters.

        The default value is 0.
    */
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)

    /*!
        \property QMcpStringSchema::minLength
        \brief The smallest accepted number of characters.

        The default value is 0.
    */
    Q_PROPERTY(int minLength READ minLength WRITE setMinLength)

    /*!
        \property QMcpStringSchema::title
        \brief A human-readable title of the requested value.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpStringSchema() : QMcpGadget(new Private) {}

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

    QString format() const {
        return d<Private>()->format;
    }

    void setFormat(const QString &format) {
        if (this->format() == format) return;
        d<Private>()->format = format;
    }

    int maxLength() const {
        return d<Private>()->maxLength;
    }

    void setMaxLength(int maxLength) {
        if (this->maxLength() == maxLength) return;
        d<Private>()->maxLength = maxLength;
    }

    int minLength() const {
        return d<Private>()->minLength;
    }

    void setMinLength(int minLength) {
        if (this->minLength() == minLength) return;
        d<Private>()->minLength = minLength;
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

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "defaultValue")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    static QString jsonKey() { return QStringLiteral("default"); }
    static QString propertyKey() { return QStringLiteral("defaultValue"); }

    struct Private : public QMcpGadget::Private {
        QString defaultValue;
        QString description;
        QString format;
        int maxLength = 0;
        int minLength = 0;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpStringSchema)

QT_END_NAMESPACE

#endif // QMCPSTRINGSCHEMA_H
