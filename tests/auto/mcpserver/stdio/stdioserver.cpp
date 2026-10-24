// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QCoreApplication>
#include <QtCore/QPointer>
#include <QtMcpServer/QMcpServer>
#include <QtMcpServer/QMcpServerBackendInterface>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QPointer<QMcpServer> server = new QMcpServer(QStringLiteral("stdio"));
    QObject::connect(server, &QMcpServer::finished, &app, &QCoreApplication::quit);
    // Exits with 0 once started() arrives, without serving any request.
    if (app.arguments().contains("--exit-on-started"_L1)) {
        QObject::connect(server, &QMcpServer::started, &app, &QCoreApplication::quit);
        QObject::connect(server, &QMcpServer::finished, &app, [] { QCoreApplication::exit(1); });
    }
    // Answers the first request, then deletes the server from within the
    // signal the backend received it with and exits.
    if (app.arguments().contains("--delete-on-received"_L1)) {
        auto backend = server->findChild<QMcpServerBackendInterface *>();
        QObject::connect(backend, &QMcpServerBackendInterface::received, &app, [&server] {
            delete server;
            QCoreApplication::quit();
        });
    }
    server->start();
    const int result = app.exec();
    delete server;
    return result;
}
