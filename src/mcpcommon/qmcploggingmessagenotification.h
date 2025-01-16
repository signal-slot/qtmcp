// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLOGGINGMESSAGENOTIFICATION_H
#define QMCPLOGGINGMESSAGENOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcploggingmessagenotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpLoggingMessageNotification
    \inmodule QtMcpCommon
    \brief Notification of a log message passed from server to client. If no logging/setLevel request has been sent from the client, the server MAY decide which messages to send automatically.
*/
class Q_MCPCOMMON_EXPORT QMcpLoggingMessageNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QString method READ method CONSTANT REQUIRED)

    Q_PROPERTY(QMcpLoggingMessageNotificationParams params READ params WRITE setParams REQUIRED)

public:
    QMcpLoggingMessageNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/message"_L1;
    }

    QMcpLoggingMessageNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpLoggingMessageNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpLoggingMessageNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPLOGGINGMESSAGENOTIFICATION_H
