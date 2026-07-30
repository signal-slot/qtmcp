// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSUBSCRIPTIONSACKNOWLEDGEDNOTIFICATIONPARAMS_H
#define QMCPSUBSCRIPTIONSACKNOWLEDGEDNOTIFICATIONPARAMS_H

#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcpsubscriptionfilter.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSubscriptionsAcknowledgedNotificationParams
    \inmodule QtMcpCommon
    \brief Parameters for a notifications/subscriptions/acknowledged notification.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpSubscriptionsAcknowledgedNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpSubscriptionsAcknowledgedNotificationParams::notifications
        \brief The subset of requested notification types the server agreed to honor.

        Only includes notification types the server actually supports; if the
        client requested an unsupported type (for example, promptsListChanged
        when the server has no prompts), it is omitted from this set.
    */
    Q_PROPERTY(QMcpSubscriptionFilter notifications READ notifications WRITE setNotifications REQUIRED)

public:
    QMcpSubscriptionsAcknowledgedNotificationParams() : QMcpNotificationParams(new Private) {}

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
    struct Private : public QMcpNotificationParams::Private {
        QMcpSubscriptionFilter notifications;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSubscriptionsAcknowledgedNotificationParams)

QT_END_NAMESPACE

#endif // QMCPSUBSCRIPTIONSACKNOWLEDGEDNOTIFICATIONPARAMS_H
