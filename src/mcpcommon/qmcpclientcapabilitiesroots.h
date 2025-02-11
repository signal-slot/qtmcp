// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTCAPABILITIESROOTS_H
#define QMCPCLIENTCAPABILITIESROOTS_H

#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpClientCapabilitiesRoots
    \inmodule QtMcpCommon
    \brief Present if the client supports listing roots.
*/
class Q_MCPCOMMON_EXPORT QMcpClientCapabilitiesRoots : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpClientCapabilitiesRoots::listChanged
        \brief Whether the client supports notifications for changes to the roots list.

        The default value is false.
    */
    Q_PROPERTY(bool listChanged READ listChanged WRITE setListChanged)

public:
    QMcpClientCapabilitiesRoots() : QMcpGadget(new Private) {}

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

Q_DECLARE_SHARED(QMcpClientCapabilitiesRoots)

QT_END_NAMESPACE

#endif // QMCPCLIENTCAPABILITIESROOTS_H
