// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPUNTITLEDMULTISELECTENUMSCHEMAITEMS_H
#define QMCPUNTITLEDMULTISELECTENUMSCHEMAITEMS_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpUntitledMultiSelectEnumSchemaItems
    \inmodule QtMcpCommon
    \brief The schema of the items of a QMcpUntitledMultiSelectEnumSchema.

    \note The schema calls the property holding the accepted values "enum",
    which is a C++ keyword and therefore cannot be used as a Q_PROPERTY name.
    The property is named enumValues instead and the JSON key is translated by
    fromJsonObject() and toJsonObject().

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpUntitledMultiSelectEnumSchemaItems : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpUntitledMultiSelectEnumSchemaItems::enumValues
        \brief The values to choose from.

        This property is serialized as "enum".
    */
    Q_PROPERTY(QList<QString> enumValues READ enumValues WRITE setEnumValues REQUIRED)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpUntitledMultiSelectEnumSchemaItems() : QMcpGadget(new Private) {}

    QList<QString> enumValues() const {
        return d<Private>()->enumValues;
    }

    void setEnumValues(const QList<QString> &enumValues) {
        if (this->enumValues() == enumValues) return;
        d<Private>()->enumValues = enumValues;
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

    struct Private : public QMcpGadget::Private {
        QList<QString> enumValues;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpUntitledMultiSelectEnumSchemaItems)

QT_END_NAMESPACE

#endif // QMCPUNTITLEDMULTISELECTENUMSCHEMAITEMS_H
