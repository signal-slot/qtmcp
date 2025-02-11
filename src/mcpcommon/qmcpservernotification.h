// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERNOTIFICATION_H
#define QMCPSERVERNOTIFICATION_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcpcancellednotification.h>
#include <QtMcpCommon/qmcpprogressnotification.h>
#include <QtMcpCommon/qmcpresourcelistchangednotification.h>
#include <QtMcpCommon/qmcpresourceupdatednotification.h>
#include <QtMcpCommon/qmcppromptlistchangednotification.h>
#include <QtMcpCommon/qmcptoollistchangednotification.h>
#include <QtMcpCommon/qmcploggingmessagenotification.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpServerNotification : public QMcpAnyOf
{
    Q_GADGET
public:
    QMcpServerNotification() : QMcpAnyOf(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpCancelledNotification cancelledNotification;
        QMcpProgressNotification progressNotification;
        QMcpResourceListChangedNotification resourceListChangedNotification;
        QMcpResourceUpdatedNotification resourceUpdatedNotification;
        QMcpPromptListChangedNotification promptListChangedNotification;
        QMcpToolListChangedNotification toolListChangedNotification;
        QMcpLoggingMessageNotification loggingMessageNotification;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpServerNotification)

QT_END_NAMESPACE

#endif // QMCPSERVERNOTIFICATION_H
