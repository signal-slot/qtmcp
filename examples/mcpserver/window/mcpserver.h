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
    explicit McpServer(QObject *parent = nullptr);
    ~McpServer() override;

    Q_INVOKABLE QImage screenShot() const;
    Q_INVOKABLE void moveCursor(int x, int y);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // MCPSERVER_H
