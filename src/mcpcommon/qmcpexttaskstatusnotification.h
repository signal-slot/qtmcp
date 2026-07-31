// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTTASKSTATUSNOTIFICATION_H
#define QMCPEXTTASKSTATUSNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcpexttaskstatusnotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtTaskStatusNotification
    \inmodule QtMcpCommon
    \brief An optional notification pushing the current state of a task to the client.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so none of its members are version gated.

    It is deliberately separate from QMcpTaskStatusNotification, which
    implements the MCP 2025-11-25 core type of the same schema name: the two are
    not wire compatible. The extension renamed the method from
    \c notifications/tasks/status to \c notifications/tasks, and the core type
    returns its method name from a \c final member function, precisely because a
    notification type stands for exactly one method.

    Servers may send these in addition to servicing client polls. Clients start
    listening by naming the task IDs they care about in a subscriptions/listen
    request. Progress and log notifications are not supported on tasks.

    \sa QMcpTaskStatusNotification, QMcpExtTaskStatusNotificationParams
*/
class Q_MCPCOMMON_EXPORT QMcpExtTaskStatusNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpExtTaskStatusNotificationParams params READ params WRITE setParams REQUIRED)

public:
    QMcpExtTaskStatusNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/tasks"_L1;
    }

    QMcpExtTaskStatusNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpExtTaskStatusNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpExtTaskStatusNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpExtTaskStatusNotification)

QT_END_NAMESPACE

#endif // QMCPEXTTASKSTATUSNOTIFICATION_H
