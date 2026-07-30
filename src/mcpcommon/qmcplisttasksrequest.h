// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTTASKSREQUEST_H
#define QMCPLISTTASKSREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcppaginatedrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListTasksRequest
    \inmodule QtMcpCommon
    \brief A request to retrieve a list of tasks.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpListTasksRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpPaginatedRequestParams params READ params WRITE setParams)

public:
    QMcpListTasksRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "tasks/list"_L1; }

    QMcpPaginatedRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpPaginatedRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpPaginatedRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListTasksRequest)

QT_END_NAMESPACE

#endif // QMCPLISTTASKSREQUEST_H
