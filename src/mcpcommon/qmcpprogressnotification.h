// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROGRESSNOTIFICATION_H
#define QMCPPROGRESSNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcpprogressnotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpProgressNotification
    \inmodule QtMcpCommon
    \brief An out-of-band notification used to inform the receiver of a progress update for a long-running request.
*/
class Q_MCPCOMMON_EXPORT QMcpProgressNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpProgressNotificationParams params READ params WRITE setParams REQUIRED)

public:
    QMcpProgressNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/progress"_L1;
    }

    QMcpProgressNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpProgressNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpProgressNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPPROGRESSNOTIFICATION_H
