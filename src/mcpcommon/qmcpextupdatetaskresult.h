// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTUPDATETASKRESULT_H
#define QMCPEXTUPDATETASKRESULT_H

#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtUpdateTaskResult
    \inmodule QtMcpCommon
    \brief The empty acknowledgement of a tasks/update request.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so none of its members are version gated. MCP
    2025-11-25 has no tasks/update method, so there is no core counterpart this
    could have extended.

    The schema defines the result as a bare \c Result, so this type adds no
    members of its own; it exists to give the response of tasks/update a name of
    its own instead of leaving callers with a plain QMcpResult. \l resultType is
    \c "complete", the standard result shape of the request.

    The acknowledgement is eventually consistent: the server may accept the
    responses and return it before the observable status of the task reflects
    them.

    \sa QMcpExtUpdateTaskRequest
*/
class Q_MCPCOMMON_EXPORT QMcpExtUpdateTaskResult : public QMcpResult
{
    Q_GADGET

public:
    QMcpExtUpdateTaskResult() : QMcpResult(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpExtUpdateTaskResult)

QT_END_NAMESPACE

#endif // QMCPEXTUPDATETASKRESULT_H
