// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOLINPUTSCHEMA_H
#define QMCPTOOLINPUTSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpToolInputSchema
    \inmodule QtMcpCommon
    \brief A JSON Schema object defining the expected parameters for the tool.
*/
class Q_MCPCOMMON_EXPORT QMcpToolInputSchema : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject properties READ properties WRITE setProperties)
    Q_PROPERTY(QList<QString> required READ required WRITE setRequired)
    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpToolInputSchema() : QMcpGadget(new Private) {}

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

QT_END_NAMESPACE

#endif // QMCPTOOLINPUTSCHEMA_H
