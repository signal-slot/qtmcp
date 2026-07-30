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

    \note The schema calls the property holding the accepted values "enum",
    which is a C++ keyword and therefore cannot be used as a Q_PROPERTY name.
    The property is named enumValues instead and the JSON key is translated by
    fromJsonObject() and toJsonObject().

    \sa QMcpPrimitiveSchemaDefinition
*/
class Q_MCPCOMMON_EXPORT QMcpEnumSchema : public QMcpGadget
{
    Q_GADGET

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
        return QMcpGadget::fromJsonObject(renamedKey(object, jsonKey(), propertyKey()), protocolVersion);
    }

    QJsonObject toJsonObject(QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest) const override {
        return renamedKey(QMcpGadget::toJsonObject(protocolVersion), propertyKey(), jsonKey());
    }

private:
    static QString jsonKey() { return QStringLiteral("enum"); }
    static QString propertyKey() { return QStringLiteral("enumValues"); }

    static QJsonObject renamedKey(QJsonObject object, const QString &from, const QString &to) {
        if (object.contains(from)) {
            object.insert(to, object.value(from));
            object.remove(from);
        }
        return object;
    }

    struct Private : public QMcpGadget::Private {
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
