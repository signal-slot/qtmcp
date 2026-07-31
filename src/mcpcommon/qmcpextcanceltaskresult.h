// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTCANCELTASKRESULT_H
#define QMCPEXTCANCELTASKRESULT_H

#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtCancelTaskResult
    \inmodule QtMcpCommon
    \brief The empty acknowledgement of a tasks/cancel request.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so none of its members are version gated.

    It is deliberately separate from QMcpCancelTaskResult, which implements the
    MCP 2025-11-25 core type of the same schema name: the two are not wire
    compatible. The core version is an allOf of Result and Task and therefore
    marks the whole task state, down to \c taskId and \c ttl, as required, while
    the extension reduced the response to a bare \c Result. Adding the task
    members to one shared type is not an option in either direction: they would
    have to stay required for 2025-11-25 yet be absent for the extension, whose
    variant forbids additional properties.

    Cancellation is cooperative and eventually consistent. The server is only
    obliged to acknowledge the request, so an empty result says nothing about
    whether the task will actually reach the \c cancelled status; the client
    reads the outcome from QMcpExtGetTaskResult, or simply drops the task state.

    \l resultType is \c "complete", the standard result shape of the request.

    \sa QMcpCancelTaskResult, QMcpCancelTaskRequest
*/
class Q_MCPCOMMON_EXPORT QMcpExtCancelTaskResult : public QMcpResult
{
    Q_GADGET

public:
    QMcpExtCancelTaskResult() : QMcpResult(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpExtCancelTaskResult)

QT_END_NAMESPACE

#endif // QMCPEXTCANCELTASKRESULT_H
