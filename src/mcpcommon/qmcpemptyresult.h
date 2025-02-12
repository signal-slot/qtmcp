// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEMPTYRESULT_H
#define QMCPEMPTYRESULT_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpEmptyResult : public QMcpResult
{
    using QMcpResult::QMcpResult;
};

QT_END_NAMESPACE

#endif // QMCPEMPTYRESULT_H
