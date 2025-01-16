// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERCAPABILITIESRESOURCES_H
#define QMCPSERVERCAPABILITIESRESOURCES_H

#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpServerCapabilitiesResources
    \inmodule QtMcpCommon
    \brief Present if the server offers any resources to read.
*/
class Q_MCPCOMMON_EXPORT QMcpServerCapabilitiesResources : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpServerCapabilitiesResources::listChanged
        \brief Whether this server supports notifications for changes to the resource list.

        The default value is false.
    */
    Q_PROPERTY(bool listChanged READ listChanged WRITE setListChanged)

    /*!
        \property QMcpServerCapabilitiesResources::subscribe
        \brief Whether this server supports subscribing to resource updates.

        The default value is false.
    */
    Q_PROPERTY(bool subscribe READ subscribe WRITE setSubscribe)

public:
    QMcpServerCapabilitiesResources() : QMcpGadget(new Private) {}

    bool listChanged() const {
        return d<Private>()->listChanged;
    }

    void setListChanged(bool changed) {
        if (this->listChanged() == changed) return;
        d<Private>()->listChanged = changed;
    }

    bool subscribe() const {
        return d<Private>()->subscribe;
    }

    void setSubscribe(bool subscribe) {
        if (this->subscribe() == subscribe) return;
        d<Private>()->subscribe = subscribe;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        bool listChanged = false;
        bool subscribe = false;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPSERVERCAPABILITIESRESOURCES_H
