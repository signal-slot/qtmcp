// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERCAPABILITIESTOOLS_H
#define QMCPSERVERCAPABILITIESTOOLS_H

#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpServerCapabilitiesTools
    \inmodule QtMcpCommon
    \brief Present if the server offers any tools to call.
*/
class Q_MCPCOMMON_EXPORT QMcpServerCapabilitiesTools : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpServerCapabilitiesTools::listChanged
        \brief Whether this server supports notifications for changes to the tool list.

        The default value is false.
    */
    Q_PROPERTY(bool listChanged READ listChanged WRITE setListChanged)

public:
    QMcpServerCapabilitiesTools() : QMcpGadget(new Private) {}

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

Q_DECLARE_SHARED(QMcpServerCapabilitiesTools)

QT_END_NAMESPACE

#endif // QMCPSERVERCAPABILITIESTOOLS_H
