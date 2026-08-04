// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSUBSCRIPTIONSLISTENREQUEST_H
#define QMCPSUBSCRIPTIONSLISTENREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpsubscriptionslistenrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSubscriptionsListenRequest
    \inmodule QtMcpCommon
    \brief Sent from the client to open a long-lived channel for receiving notifications outside the context of a specific request.

    Replaces the previous HTTP GET endpoint and ensures consistent behavior
    between HTTP and STDIO.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpSubscriptionsListenRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpSubscriptionsListenRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpSubscriptionsListenRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "subscriptions/listen"_L1; }

    QMcpSubscriptionsListenRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpSubscriptionsListenRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpSubscriptionsListenRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSubscriptionsListenRequest)

QT_END_NAMESPACE

#endif // QMCPSUBSCRIPTIONSLISTENREQUEST_H
