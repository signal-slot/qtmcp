// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPNUMBERSCHEMA_H
#define QMCPNUMBERSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpNumberSchema
    \inmodule QtMcpCommon
    \brief A restricted JSON Schema for a numeric value requested via elicitation.

    \note The schema calls the property holding the initial value "default",
    which is a C++ keyword and therefore cannot be used as a Q_PROPERTY name.
    The property is named defaultValue instead and the JSON key is translated
    by fromJsonObject() and toJsonObject().

    \sa QMcpPrimitiveSchemaDefinition
*/
class Q_MCPCOMMON_EXPORT QMcpNumberSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpNumberSchema::defaultValue
        \brief The value to be used unless the user enters another one.

        This property is serialized as "default". The default value is 0.
        \since MCP 2025-11-25
    */
    Q_PROPERTY(qreal defaultValue READ defaultValue WRITE setDefaultValue)

    /*!
        \property QMcpNumberSchema::description
        \brief A human-readable description of the requested value.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpNumberSchema::maximum
        \brief The largest accepted value.

        The default value is 0.
    */
    Q_PROPERTY(qreal maximum READ maximum WRITE setMaximum)

    /*!
        \property QMcpNumberSchema::minimum
        \brief The smallest accepted value.

        The default value is 0.
    */
    Q_PROPERTY(qreal minimum READ minimum WRITE setMinimum)

    /*!
        \property QMcpNumberSchema::title
        \brief A human-readable title of the requested value.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    /*!
        \property QMcpNumberSchema::type
        \brief Either "number" for a floating point value or "integer" for an integral one.

        The default value is "number".
    */
    Q_PROPERTY(QString type READ type WRITE setType REQUIRED)

public:
    QMcpNumberSchema() : QMcpGadget(new Private) {}

    qreal defaultValue() const {
        return d<Private>()->defaultValue;
    }

    void setDefaultValue(qreal defaultValue) {
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

    qreal maximum() const {
        return d<Private>()->maximum;
    }

    void setMaximum(qreal maximum) {
        if (this->maximum() == maximum) return;
        d<Private>()->maximum = maximum;
    }

    qreal minimum() const {
        return d<Private>()->minimum;
    }

    void setMinimum(qreal minimum) {
        if (this->minimum() == minimum) return;
        d<Private>()->minimum = minimum;
    }

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
    }

    QString type() const {
        return d<Private>()->type;
    }

    void setType(const QString &type) {
        if (this->type() == type) return;
        d<Private>()->type = type;
    }

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
        qreal defaultValue = 0;
        QString description;
        qreal maximum = 0;
        qreal minimum = 0;
        QString title;
        QString type = QStringLiteral("number");

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpNumberSchema)

QT_END_NAMESPACE

#endif // QMCPNUMBERSCHEMA_H
