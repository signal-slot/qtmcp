// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSUBSCRIBEREQUEST_H
#define QMCPSUBSCRIBEREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpsubscriberequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSubscribeRequest
    \inmodule QtMcpCommon
    \brief Sent from the client to request resources/updated notifications from the server whenever a particular resource changes.
*/
class Q_MCPCOMMON_EXPORT QMcpSubscribeRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpSubscribeRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpSubscribeRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "resources/subscribe"_L1; }

    QMcpSubscribeRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpSubscribeRequestParams& params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpSubscribeRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSubscribeRequest)

QT_END_NAMESPACE

#endif // QMCPSUBSCRIBEREQUEST_H
