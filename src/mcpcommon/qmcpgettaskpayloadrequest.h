// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPGETTASKPAYLOADREQUEST_H
#define QMCPGETTASKPAYLOADREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpgettaskpayloadrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpGetTaskPayloadRequest
    \inmodule QtMcpCommon
    \brief A request to retrieve the result of a completed task.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpGetTaskPayloadRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpGetTaskPayloadRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpGetTaskPayloadRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "tasks/result"_L1; }

    QMcpGetTaskPayloadRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpGetTaskPayloadRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpGetTaskPayloadRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpGetTaskPayloadRequest)

QT_END_NAMESPACE

#endif // QMCPGETTASKPAYLOADREQUEST_H
