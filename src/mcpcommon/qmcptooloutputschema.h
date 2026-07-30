// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOLOUTPUTSCHEMA_H
#define QMCPTOOLOUTPUTSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpToolOutputSchema
    \inmodule QtMcpCommon
    \brief A JSON Schema object defining the structure of the tool's output returned in the structuredContent field of a QMcpCallToolResult.

    \since MCP 2025-06-18
*/
class Q_MCPCOMMON_EXPORT QMcpToolOutputSchema : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject properties READ properties WRITE setProperties)
    Q_PROPERTY(QList<QString> required READ required WRITE setRequired)
    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpToolOutputSchema() : QMcpGadget(new Private) {}

    QJsonObject properties() const {
        return d<Private>()->properties;
    }

    void setProperties(const QJsonObject &properties) {
        if (this->properties() == properties) return;
        d<Private>()->properties = properties;
    }

    QList<QString> required() const {
        return d<Private>()->required;
    }

    void setRequired(const QList<QString> &required) {
        if (this->required() == required) return;
        d<Private>()->required = required;
    }

    static QByteArray type() {
        return QByteArrayLiteral("object");
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject properties;
        QList<QString> required;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpToolOutputSchema)

QT_END_NAMESPACE

#endif // QMCPTOOLOUTPUTSCHEMA_H
