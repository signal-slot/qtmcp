// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTASKSTATUSNOTIFICATION_H
#define QMCPTASKSTATUSNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcptaskstatusnotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTaskStatusNotification
    \inmodule QtMcpCommon
    \brief An optional notification from the receiver to the requestor, informing them that a task's status has changed.

    Receivers are not required to send these notifications.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpTaskStatusNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpTaskStatusNotificationParams params READ params WRITE setParams REQUIRED)

public:
    QMcpTaskStatusNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/tasks/status"_L1;
    }

    QMcpTaskStatusNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpTaskStatusNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpTaskStatusNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTaskStatusNotification)

QT_END_NAMESPACE

#endif // QMCPTASKSTATUSNOTIFICATION_H
