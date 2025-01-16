// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLOGGINGLEVEL_H
#define QMCPLOGGINGLEVEL_H

#include <QtMcpCommon/qmcpcommonglobal.h>

QT_BEGIN_NAMESPACE

namespace QMcpLoggingLevel {
Q_NAMESPACE_EXPORT(Q_MCPCOMMON_EXPORT)

enum Q_MCPCOMMON_EXPORT QMcpLoggingLevel {
    alert,
    critical,
    debug,
    emergency,
    error,
    info,
    notice,
    warning,
};

Q_ENUM_NS(QMcpLoggingLevel)
}

QT_END_NAMESPACE

#endif // QMCPLOGGINGLEVEL_H
