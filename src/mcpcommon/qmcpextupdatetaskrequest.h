// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTUPDATETASKREQUEST_H
#define QMCPEXTUPDATETASKREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpextupdatetaskrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtUpdateTaskRequest
    \inmodule QtMcpCommon
    \brief A request answering the outstanding input requests of a task.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so none of its members are version gated. MCP
    2025-11-25 has no tasks/update method, so there is no core counterpart this
    could have extended.

    The client sends it after observing a task in the \c input_required status.
    The acknowledgement is eventually consistent: the server may return it
    before the observable status of the task reflects the responses.

    Over the Streamable HTTP transport the client must set the \c Mcp-Name
    header to the value of \c params.taskId so that intermediaries can route the
    request to the server instance holding the task state.

    \sa QMcpExtUpdateTaskRequestParams, QMcpExtUpdateTaskResult
*/
class Q_MCPCOMMON_EXPORT QMcpExtUpdateTaskRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpExtUpdateTaskRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpExtUpdateTaskRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "tasks/update"_L1; }

    QMcpExtUpdateTaskRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpExtUpdateTaskRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpExtUpdateTaskRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpExtUpdateTaskRequest)

QT_END_NAMESPACE

#endif // QMCPEXTUPDATETASKREQUEST_H
