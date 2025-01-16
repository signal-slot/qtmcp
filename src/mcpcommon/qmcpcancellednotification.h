// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCANCELLEDNOTIFICATION_H
#define QMCPCANCELLEDNOTIFICATION_H

#include <QtMcpCommon/qmcpcancellednotificationparams.h>
#include <QtMcpCommon/qmcpnotification.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCancelledNotification
    \inmodule QtMcpCommon
    \brief This notification can be sent by either side to indicate that it is cancelling a previously-issued request.
    
    The request SHOULD still be in-flight, but due to communication latency, it is always possible that this notification MAY arrive after the request has already finished.
    
    This notification indicates that the result will be unused, so any associated processing SHOULD cease.
    
    A client MUST NOT attempt to cancel its `initialize` request.
*/
class Q_MCPCOMMON_EXPORT QMcpCancelledNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpCancelledNotificationParams params READ params WRITE setParams REQUIRED)

public:
    QMcpCancelledNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/cancelled"_L1;
    }

    QMcpCancelledNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpCancelledNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpCancelledNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCANCELLEDNOTIFICATION_H
