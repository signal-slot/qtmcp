// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTITLEDSINGLESELECTENUMSCHEMAONEOF_H
#define QMCPTITLEDSINGLESELECTENUMSCHEMAONEOF_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTitledSingleSelectEnumSchemaOneOf
    \inmodule QtMcpCommon
    \brief One option of a QMcpTitledSingleSelectEnumSchema.

    \note The schema calls the property holding the value "const", which is a
    C++ keyword and therefore cannot be used as a Q_PROPERTY name. The property
    is named constValue instead and the JSON key is translated by
    fromJsonObject() and toJsonObject().

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpTitledSingleSelectEnumSchemaOneOf : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpTitledSingleSelectEnumSchemaOneOf::constValue
        \brief The value this option stands for.

        This property is serialized as "const".
    */
    Q_PROPERTY(QString constValue READ constValue WRITE setConstValue REQUIRED)

    /*!
        \property QMcpTitledSingleSelectEnumSchemaOneOf::title
        \brief The label to display for this option.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle REQUIRED)

public:
    QMcpTitledSingleSelectEnumSchemaOneOf() : QMcpGadget(new Private) {}

    QString constValue() const {
        return d<Private>()->constValue;
    }

    void setConstValue(const QString &constValue) {
        if (this->constValue() == constValue) return;
        d<Private>()->constValue = constValue;
    }

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
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

private:
    static QString jsonKey() { return QStringLiteral("const"); }
    static QString propertyKey() { return QStringLiteral("constValue"); }

    struct Private : public QMcpGadget::Private {
        QString constValue;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTitledSingleSelectEnumSchemaOneOf)

QT_END_NAMESPACE

#endif // QMCPTITLEDSINGLESELECTENUMSCHEMAONEOF_H
