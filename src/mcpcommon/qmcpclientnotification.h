// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTNOTIFICATION_H
#define QMCPCLIENTNOTIFICATION_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcpcancellednotification.h>
#include <QtMcpCommon/qmcpinitializednotification.h>
#include <QtMcpCommon/qmcpprogressnotification.h>
#include <QtMcpCommon/qmcprootslistchangednotification.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpClientNotification : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpCancelledNotification cancelledNotification READ cancelledNotification WRITE setCancelledNotification)
    Q_PROPERTY(QMcpInitializedNotification initializedNotification READ initializedNotification WRITE setInitializedNotification)
    Q_PROPERTY(QMcpProgressNotification progressNotification READ progressNotification WRITE setProgressNotification)
    Q_PROPERTY(QMcpRootsListChangedNotification rootsListChangedNotification READ rootsListChangedNotification WRITE setRootsListChangedNotification)

public:
    QMcpClientNotification() : QMcpAnyOf(new Private) {}

    QMcpCancelledNotification cancelledNotification() const {
        return d<Private>()->cancelledNotification;
    }

    void setCancelledNotification(const QMcpCancelledNotification &cancelledNotification) {
        if (this->cancelledNotification() == cancelledNotification) return;
        setRefType("cancelledNotification"_ba);
        d<Private>()->cancelledNotification = cancelledNotification;
    }

    QMcpInitializedNotification initializedNotification() const {
        return d<Private>()->initializedNotification;
    }

    void setInitializedNotification(const QMcpInitializedNotification &initializedNotification) {
        if (this->initializedNotification() == initializedNotification) return;
        setRefType("initializedNotification"_ba);
        d<Private>()->initializedNotification = initializedNotification;
    }

    QMcpProgressNotification progressNotification() const {
        return d<Private>()->progressNotification;
    }

    void setProgressNotification(const QMcpProgressNotification &progressNotification) {
        if (this->progressNotification() == progressNotification) return;
        setRefType("progressNotification"_ba);
        d<Private>()->progressNotification = progressNotification;
    }

    QMcpRootsListChangedNotification rootsListChangedNotification() const {
        return d<Private>()->rootsListChangedNotification;
    }

    void setRootsListChangedNotification(const QMcpRootsListChangedNotification &rootsListChangedNotification) {
        if (this->rootsListChangedNotification() == rootsListChangedNotification) return;
        setRefType("rootsListChangedNotification"_ba);
        d<Private>()->rootsListChangedNotification = rootsListChangedNotification;
    }

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpCancelledNotification cancelledNotification;
        QMcpInitializedNotification initializedNotification;
        QMcpProgressNotification progressNotification;
        QMcpRootsListChangedNotification rootsListChangedNotification;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpClientNotification)

QT_END_NAMESPACE

#endif // QMCPCLIENTNOTIFICATION_H
