// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPUNSUBSCRIBEREQUEST_H
#define QMCPUNSUBSCRIBEREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpunsubscriberequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpUnsubscribeRequest
    \inmodule QtMcpCommon
    \brief Sent from the client to request cancellation of resources/updated notifications from the server. This should follow a previous resources/subscribe request.
*/
class Q_MCPCOMMON_EXPORT QMcpUnsubscribeRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpUnsubscribeRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpUnsubscribeRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "resources/unsubscribe"_L1; }

    QMcpUnsubscribeRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpUnsubscribeRequestParams& params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpUnsubscribeRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpUnsubscribeRequest)

QT_END_NAMESPACE

#endif // QMCPUNSUBSCRIBEREQUEST_H
