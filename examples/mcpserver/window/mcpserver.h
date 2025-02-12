// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef MCPSERVER_H
#define MCPSERVER_H

#include <QtMcpServer/QMcpServer>
#include <QtGui/QImage>

class McpServer : public QMcpServer
{
    Q_OBJECT
public:
    explicit McpServer(const QString &backend = "stdio", QObject *parent = nullptr);

    Q_INVOKABLE QImage screenShot() const;
    Q_INVOKABLE void moveCursor(int x, int y);
};

#endif // MCPSERVER_H
