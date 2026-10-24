// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QCoreApplication>
#include <QtMcpServer/QMcpServer>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QMcpServer server(QStringLiteral("stdio"));
    QObject::connect(&server, &QMcpServer::finished, &app, &QCoreApplication::quit);
    server.start();
    return app.exec();
}
