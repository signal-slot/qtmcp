// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPREQUESTID_H
#define QMCPREQUESTID_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

/*!
    \typedef QMcpRequestId
    \inmodule QtMcpCommon
    \brief A uniquely identifying ID for a request in JSON-RPC.
*/
using QMcpRequestId = QVariant Q_MCPCOMMON_EXPORT;

inline size_t qHash(const QMcpRequestId &id, size_t seed)
{
    switch (id.typeId()) {
    case QMetaType::Int:
        return qHash(id.toInt(), seed);
    case QMetaType::LongLong:
        return qHash(id.toLongLong(), seed);
    case QMetaType::QString:
        return qHash(id.toString(), seed);
    default:
        qFatal() << id.typeId() << "is not supported for RequestId";
    }
    return 0;
}

QT_END_NAMESPACE

#endif // QMCPREQUESTID_H
