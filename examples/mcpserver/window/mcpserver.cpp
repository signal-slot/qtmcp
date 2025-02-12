// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "mcpserver.h"
#include <QtGui/QCursor>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

McpServer::McpServer(const QString &backend, QObject *parent)
    : QMcpServer(backend, parent)
{}

QImage McpServer::screenShot() const
{
    return QGuiApplication::primaryScreen()->grabWindow().toImage();
}

void McpServer::moveCursor(int x, int y)
{
    QCursor::setPos(x, y);
}
