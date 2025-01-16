// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPNOTIFICATION_H
#define QMCPNOTIFICATION_H

#include <QtMcpCommon/qmcpjsonrpcnotification.h>
#include <QtMcpCommon/qmcpnotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpNotification
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpNotification : public QMcpJSONRPCNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpNotificationParams params READ params WRITE setParams)

public:
    QMcpNotification() : QMcpJSONRPCNotification(new Private) {}
protected:
    QMcpNotification(Private *d) : QMcpJSONRPCNotification(d) {}
public:
    QMcpNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpJSONRPCNotification::Private {
        QMcpNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPNOTIFICATION_H
