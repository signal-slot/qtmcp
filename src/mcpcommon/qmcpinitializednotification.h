// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPINITIALIZEDNOTIFICATION_H
#define QMCPINITIALIZEDNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcpinitializednotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpInitializedNotification
    \inmodule QtMcpCommon
    \brief This notification is sent from the client to the server after initialization has finished.
*/
class Q_MCPCOMMON_EXPORT QMcpInitializedNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QString method READ method CONSTANT REQUIRED)
    Q_PROPERTY(QMcpInitializedNotificationParams params READ params WRITE setParams)

public:
    QMcpInitializedNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/initialized"_L1;
    }

    QMcpInitializedNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpInitializedNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpInitializedNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPINITIALIZEDNOTIFICATION_H
