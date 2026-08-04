// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPELICITATIONCOMPLETENOTIFICATION_H
#define QMCPELICITATIONCOMPLETENOTIFICATION_H

#include <QtMcpCommon/qmcpelicitationcompletenotificationparams.h>
#include <QtMcpCommon/qmcpnotification.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpElicitationCompleteNotification
    \inmodule QtMcpCommon
    \brief An optional notification from the server to the client, informing it of the completion of an out-of-band elicitation request.

    Out-of-band means an elicitation the user answered after being sent to the
    url of a QMcpElicitRequest whose mode is "url". The client cannot tell when
    the user is done there, so the server says so with this notification.

    \sa QMcpElicitRequestParams
    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpElicitationCompleteNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpElicitationCompleteNotificationParams params READ params WRITE setParams REQUIRED)

public:
    QMcpElicitationCompleteNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/elicitation/complete"_L1;
    }

    QMcpElicitationCompleteNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpElicitationCompleteNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpElicitationCompleteNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpElicitationCompleteNotification)

QT_END_NAMESPACE

#endif // QMCPELICITATIONCOMPLETENOTIFICATION_H
