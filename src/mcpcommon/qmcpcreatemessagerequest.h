// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCREATEMESSAGEREQUEST_H
#define QMCPCREATEMESSAGEREQUEST_H

#include <QtMcpCommon/qmcpcreatemessagerequestparams.h>
#include <QtMcpCommon/qmcprequest.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCreateMessageRequest
    \inmodule QtMcpCommon
    \brief A request from the server to sample an LLM via the client. The client has full discretion over which model to select. The client should also inform the user before beginning sampling, to allow them to inspect the request (human in the loop) and decide whether to approve it.
*/
class Q_MCPCOMMON_EXPORT QMcpCreateMessageRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpCreateMessageRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpCreateMessageRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "sampling/createMessage"_L1; }

    QMcpCreateMessageRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpCreateMessageRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpCreateMessageRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCreateMessageRequest)

QT_END_NAMESPACE

#endif // QMCPCREATEMESSAGEREQUEST_H
