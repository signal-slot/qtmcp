// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCANCELLEDNOTIFICATIONPARAMS_H
#define QMCPCANCELLEDNOTIFICATIONPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcprequestid.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCancelledNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpCancelledNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpCancelledNotificationParams::reason
        \brief An optional string describing the reason for the cancellation. This MAY be logged or presented to the user.
    */
    Q_PROPERTY(QString reason READ reason WRITE setReason)

    /*!
        \property QMcpCancelledNotificationParams::requestId
        \brief The ID of the request to cancel.
        
        This MUST correspond to the ID of a request previously issued in the same direction.
    */
    Q_PROPERTY(QMcpRequestId requestId READ requestId WRITE setRequestId REQUIRED)

public:
    QMcpCancelledNotificationParams() : QMcpNotificationParams(new Private) {}

    QString reason() const {
        return d<Private>()->reason;
    }

    void setReason(const QString &reason) {
        if (this->reason() == reason) return;
        d<Private>()->reason = reason;
    }

    QMcpRequestId requestId() const {
        return d<Private>()->requestId;
    }

    void setRequestId(const QMcpRequestId &requestId) {
        if (this->requestId() == requestId) return;
        d<Private>()->requestId = requestId;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QString reason;
        QMcpRequestId requestId;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCANCELLEDNOTIFICATIONPARAMS_H
