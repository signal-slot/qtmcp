// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPROLE_H
#define QMCPROLE_H

#include <QtMcpCommon/qmcpcommonglobal.h>

QT_BEGIN_NAMESPACE

namespace QMcpRole {
Q_NAMESPACE_EXPORT(Q_MCPCOMMON_EXPORT)

enum Q_MCPCOMMON_EXPORT QMcpRole {
    assistant,
    user,
};

Q_ENUM_NS(QMcpRole)

}

QT_END_NAMESPACE

#endif // QMCPROLE_H
