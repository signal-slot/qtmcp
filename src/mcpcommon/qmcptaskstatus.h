// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTASKSTATUS_H
#define QMCPTASKSTATUS_H

#include <QtMcpCommon/qmcpcommonglobal.h>

QT_BEGIN_NAMESPACE

/*! \namespace QMcpTaskStatus
    \inmodule QtMcpCommon
    \brief The status of a task.

    \since MCP 2025-11-25
*/
namespace QMcpTaskStatus {
Q_NAMESPACE_EXPORT(Q_MCPCOMMON_EXPORT)

enum Q_MCPCOMMON_EXPORT QMcpTaskStatus {
    cancelled,
    completed,
    failed,
    input_required,
    working,
};

Q_ENUM_NS(QMcpTaskStatus)

}

QT_END_NAMESPACE

#endif // QMCPTASKSTATUS_H
