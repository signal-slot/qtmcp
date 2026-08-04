// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTITLEDMULTISELECTENUMSCHEMAITEMS_H
#define QMCPTITLEDMULTISELECTENUMSCHEMAITEMS_H

#include <QtCore/QList>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcptitledmultiselectenumschemaitemsanyof.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTitledMultiSelectEnumSchemaItems
    \inmodule QtMcpCommon
    \brief The schema of the items of a QMcpTitledMultiSelectEnumSchema.

    Unlike QMcpUntitledMultiSelectEnumSchemaItems this one has no "type" member:
    every option carries its own value, so the items are described by anyOf
    alone.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpTitledMultiSelectEnumSchemaItems : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpTitledMultiSelectEnumSchemaItems::anyOf
        \brief The options to choose from, each with a value and a display label.
    */
    Q_PROPERTY(QList<QMcpTitledMultiSelectEnumSchemaItemsAnyOf> anyOf READ anyOf WRITE setAnyOf REQUIRED)

public:
    QMcpTitledMultiSelectEnumSchemaItems() : QMcpGadget(new Private) {
        // QMcpGadget looks the element type of a list property up by name.
        qRegisterMetaType<QMcpTitledMultiSelectEnumSchemaItemsAnyOf>();
    }

    QList<QMcpTitledMultiSelectEnumSchemaItemsAnyOf> anyOf() const {
        return d<Private>()->anyOf;
    }

    void setAnyOf(const QList<QMcpTitledMultiSelectEnumSchemaItemsAnyOf> &anyOf) {
        if (this->anyOf() == anyOf) return;
        d<Private>()->anyOf = anyOf;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QList<QMcpTitledMultiSelectEnumSchemaItemsAnyOf> anyOf;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTitledMultiSelectEnumSchemaItems)

QT_END_NAMESPACE

#endif // QMCPTITLEDMULTISELECTENUMSCHEMAITEMS_H
