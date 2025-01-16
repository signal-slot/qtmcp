// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERCAPABILITIESPROMPTS_H
#define QMCPSERVERCAPABILITIESPROMPTS_H

#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpServerCapabilitiesPrompts
    \inmodule QtMcpCommon
    \brief Present if the server offers any prompt templates.
*/
class Q_MCPCOMMON_EXPORT QMcpServerCapabilitiesPrompts : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpServerCapabilitiesPrompts::listChanged
        \brief Whether this server supports notifications for changes to the prompt list.

        The default value is false.
    */
    Q_PROPERTY(bool listChanged READ listChanged WRITE setListChanged)

public:
    QMcpServerCapabilitiesPrompts() : QMcpGadget(new Private) {}

    bool listChanged() const {
        return d<Private>()->listChanged;
    }

    void setListChanged(bool changed) {
        if (this->listChanged() == changed) return;
        d<Private>()->listChanged = changed;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        bool listChanged = false;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPSERVERCAPABILITIESPROMPTS_H
