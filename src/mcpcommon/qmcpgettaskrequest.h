// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPGETTASKREQUEST_H
#define QMCPGETTASKREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpgettaskrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpGetTaskRequest
    \inmodule QtMcpCommon
    \brief A request to retrieve the state of a task.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpGetTaskRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpGetTaskRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpGetTaskRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "tasks/get"_L1; }

    QMcpGetTaskRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpGetTaskRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpGetTaskRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpGetTaskRequest)

QT_END_NAMESPACE

#endif // QMCPGETTASKREQUEST_H
