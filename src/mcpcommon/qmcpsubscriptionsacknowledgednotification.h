// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSUBSCRIPTIONSACKNOWLEDGEDNOTIFICATION_H
#define QMCPSUBSCRIPTIONSACKNOWLEDGEDNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcpsubscriptionsacknowledgednotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSubscriptionsAcknowledgedNotification
    \inmodule QtMcpCommon
    \brief Sent by the server to acknowledge that a subscriptions/listen subscription has been established.

    It reports which notification types the server agreed to honor. This
    notification must be the first message the server sends carrying the
    subscription's ID in \c io.modelcontextprotocol/subscriptionId, and the
    server must not send any notification on the subscription before
    acknowledging it. On stdio, where every subscription shares one channel,
    that ordering is defined per subscription ID and not per channel: messages
    belonging to other subscriptions may be interleaved before it.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpSubscriptionsAcknowledgedNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpSubscriptionsAcknowledgedNotificationParams params READ params WRITE setParams REQUIRED)

public:
    QMcpSubscriptionsAcknowledgedNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/subscriptions/acknowledged"_L1;
    }

    QMcpSubscriptionsAcknowledgedNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpSubscriptionsAcknowledgedNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpSubscriptionsAcknowledgedNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSubscriptionsAcknowledgedNotification)

QT_END_NAMESPACE

#endif // QMCPSUBSCRIPTIONSACKNOWLEDGEDNOTIFICATION_H
