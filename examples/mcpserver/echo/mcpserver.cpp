// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "mcpserver.h"
#include <QtCore/QHash>
#include <QtCore/QUuid>

McpServer::McpServer(const QString &backend, QObject *parent)
    : QMcpServer(backend, parent)
{}

QString McpServer::echo(const QString &message) const
{
    return message;
}

QHash<QString, QString> McpServer::descriptions() const
{
    return {
        { "echo"_L1, "Echoes back the input"_L1 },
        { "echo/message"_L1, "Message to echo"_L1 }
    };
}
