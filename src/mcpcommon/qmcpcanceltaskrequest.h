// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCANCELTASKREQUEST_H
#define QMCPCANCELTASKREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpcanceltaskrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCancelTaskRequest
    \inmodule QtMcpCommon
    \brief A request to cancel a task.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpCancelTaskRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpCancelTaskRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpCancelTaskRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "tasks/cancel"_L1; }

    QMcpCancelTaskRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpCancelTaskRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpCancelTaskRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCancelTaskRequest)

QT_END_NAMESPACE

#endif // QMCPCANCELTASKREQUEST_H
