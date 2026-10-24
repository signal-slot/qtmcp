// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QCoreApplication>
#include <QtMcpServer/QMcpServer>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QMcpServer server(QStringLiteral("stdio"));
    QObject::connect(&server, &QMcpServer::finished, &app, &QCoreApplication::quit);
    // Exits with 0 once started() arrives, without serving any request.
    if (app.arguments().contains("--exit-on-started"_L1)) {
        QObject::connect(&server, &QMcpServer::started, &app, &QCoreApplication::quit);
        QObject::connect(&server, &QMcpServer::finished, &app, [] { QCoreApplication::exit(1); });
    }
    server.start();
    return app.exec();
}
