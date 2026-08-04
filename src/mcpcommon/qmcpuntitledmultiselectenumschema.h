// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPUNTITLEDMULTISELECTENUMSCHEMA_H
#define QMCPUNTITLEDMULTISELECTENUMSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpuntitledmultiselectenumschemaitems.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpUntitledMultiSelectEnumSchema
    \inmodule QtMcpCommon
    \brief A restricted JSON Schema for any number of values picked from a fixed set of strings, without display titles.

    The user picks between minItems and maxItems of the values enumerated by
    items. Use QMcpTitledMultiSelectEnumSchema when the options need a display
    label.

    \note The schema calls the property holding the initial value "default",
    which is a C++ keyword and therefore cannot be used as a Q_PROPERTY name.
    The property is named defaultValue instead and the JSON key is translated
    by fromJsonObject() and toJsonObject().

    \sa QMcpPrimitiveSchemaDefinition
    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpUntitledMultiSelectEnumSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpUntitledMultiSelectEnumSchema::defaultValue
        \brief The values to be used unless the user picks others.

        This property is serialized as "default".
    */
    Q_PROPERTY(QList<QString> defaultValue READ defaultValue WRITE setDefaultValue)

    /*!
        \property QMcpUntitledMultiSelectEnumSchema::description
        \brief A human-readable description of the requested values.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpUntitledMultiSelectEnumSchema::items
        \brief The schema every picked value has to match.
    */
    Q_PROPERTY(QMcpUntitledMultiSelectEnumSchemaItems items READ items WRITE setItems REQUIRED)

    /*!
        \property QMcpUntitledMultiSelectEnumSchema::maxItems
        \brief The largest accepted number of picked values.

        The default value is 0, which leaves the number unconstrained.
    */
    Q_PROPERTY(int maxItems READ maxItems WRITE setMaxItems)

    /*!
        \property QMcpUntitledMultiSelectEnumSchema::minItems
        \brief The smallest accepted number of picked values.

        The default value is 0.
    */
    Q_PROPERTY(int minItems READ minItems WRITE setMinItems)

    /*!
        \property QMcpUntitledMultiSelectEnumSchema::title
        \brief A human-readable title of the requested values.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpUntitledMultiSelectEnumSchema() : QMcpGadget(new Private) {}

    QList<QString> defaultValue() const {
        return d<Private>()->defaultValue;
    }

    void setDefaultValue(const QList<QString> &defaultValue) {
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

    QMcpUntitledMultiSelectEnumSchemaItems items() const {
        return d<Private>()->items;
    }

    void setItems(const QMcpUntitledMultiSelectEnumSchemaItems &items) {
        if (this->items() == items) return;
        d<Private>()->items = items;
    }

    int maxItems() const {
        return d<Private>()->maxItems;
    }

    void setMaxItems(int maxItems) {
        if (this->maxItems() == maxItems) return;
        d<Private>()->maxItems = maxItems;
    }

    int minItems() const {
        return d<Private>()->minItems;
    }

    void setMinItems(int minItems) {
        if (this->minItems() == minItems) return;
        d<Private>()->minItems = minItems;
    }

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
    }

    static QByteArray type() { return QByteArrayLiteral("array"); }

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
        QList<QString> defaultValue;
        QString description;
        QMcpUntitledMultiSelectEnumSchemaItems items;
        int maxItems = 0;
        int minItems = 0;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpUntitledMultiSelectEnumSchema)

QT_END_NAMESPACE

#endif // QMCPUNTITLEDMULTISELECTENUMSCHEMA_H
