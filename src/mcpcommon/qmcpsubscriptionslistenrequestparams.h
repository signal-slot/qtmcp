// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSUBSCRIPTIONSLISTENREQUESTPARAMS_H
#define QMCPSUBSCRIPTIONSLISTENREQUESTPARAMS_H

#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpsubscriptionfilter.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSubscriptionsListenRequestParams
    \inmodule QtMcpCommon
    \brief Parameters for a subscriptions/listen request.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpSubscriptionsListenRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpSubscriptionsListenRequestParams::notifications
        \brief The notifications the client opts in to on this stream.

        The server must not send notification types the client has not
        explicitly requested.
    */
    Q_PROPERTY(QMcpSubscriptionFilter notifications READ notifications WRITE setNotifications REQUIRED)

public:
    QMcpSubscriptionsListenRequestParams() : QMcpGadget(new Private) {}

    QMcpSubscriptionFilter notifications() const {
        return d<Private>()->notifications;
    }

    void setNotifications(const QMcpSubscriptionFilter &notifications) {
        if (this->notifications() == notifications) return;
        d<Private>()->notifications = notifications;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpSubscriptionFilter notifications;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSubscriptionsListenRequestParams)

QT_END_NAMESPACE

#endif // QMCPSUBSCRIPTIONSLISTENREQUESTPARAMS_H
