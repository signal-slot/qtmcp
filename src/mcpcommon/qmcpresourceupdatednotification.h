// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCEUPDATEDNOTIFICATION_H
#define QMCPRESOURCEUPDATEDNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcpresourceupdatednotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceUpdatedNotification
    \inmodule QtMcpCommon
    \brief A notification from the server to the client, informing it that a resource has changed and may need to be read again. This should only be sent if the client previously sent a resources/subscribe request.
*/
class Q_MCPCOMMON_EXPORT QMcpResourceUpdatedNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpResourceUpdatedNotificationParams params READ params WRITE setParams REQUIRED)

public:
    QMcpResourceUpdatedNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/resources/updated"_L1;
    }

    QMcpResourceUpdatedNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpResourceUpdatedNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpResourceUpdatedNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpResourceUpdatedNotification)

QT_END_NAMESPACE

#endif // QMCPRESOURCEUPDATEDNOTIFICATION_H
