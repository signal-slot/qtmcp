// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPELICITREQUESTPARAMSREQUESTEDSCHEMA_H
#define QMCPELICITREQUESTPARAMSREQUESTEDSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpElicitRequestParamsRequestedSchema
    \inmodule QtMcpCommon
    \brief A restricted subset of JSON Schema. Only top-level properties are allowed, without nesting.

    \sa QMcpPrimitiveSchemaDefinition
*/
class Q_MCPCOMMON_EXPORT QMcpElicitRequestParamsRequestedSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpElicitRequestParamsRequestedSchema::properties
        \brief The requested values, keyed by property name.

        Each value is the JSON representation of a
        QMcpPrimitiveSchemaDefinition. The map itself is kept as a QJsonObject
        because the protocol allows arbitrary property names.
    */
    Q_PROPERTY(QJsonObject properties READ properties WRITE setProperties REQUIRED)

    /*!
        \property QMcpElicitRequestParamsRequestedSchema::required
        \brief The names of the properties the user has to fill in.
    */
    Q_PROPERTY(QList<QString> required READ required WRITE setRequired)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpElicitRequestParamsRequestedSchema() : QMcpGadget(new Private) {}

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

    static QByteArray type() { return QByteArrayLiteral("object"); }

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

Q_DECLARE_SHARED(QMcpElicitRequestParamsRequestedSchema)

QT_END_NAMESPACE

#endif // QMCPELICITREQUESTPARAMSREQUESTEDSCHEMA_H
