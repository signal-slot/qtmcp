// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef MCPSERVER_H
#define MCPSERVER_H

#include <QtMcpServer/QMcpServer>

class McpServer : public QMcpServer
{
    Q_OBJECT
public:
    explicit McpServer(QObject *parent = nullptr);
    ~McpServer() override;

    Q_INVOKABLE QString echo(const QString &message) const;

    QHash<QString, QString> descriptions() const override;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // MCPSERVER_H
