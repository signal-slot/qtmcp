// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPGETTASKPAYLOADRESULT_H
#define QMCPGETTASKPAYLOADRESULT_H

#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpGetTaskPayloadResult
    \inmodule QtMcpCommon
    \brief The response to a tasks/result request.

    The structure matches the result type of the original request. For example,
    a tools/call task returns the CallToolResult structure. The payload is
    therefore carried by the additionalProperties member inherited from
    QMcpResult and this type declares no member of its own.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpGetTaskPayloadResult : public QMcpResult
{
    Q_GADGET

public:
    QMcpGetTaskPayloadResult() : QMcpResult(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpGetTaskPayloadResult)

QT_END_NAMESPACE

#endif // QMCPGETTASKPAYLOADRESULT_H
