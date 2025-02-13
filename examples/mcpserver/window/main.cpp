// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QCommandLineParser>
#include <QtGui/QGuiApplication>
#include <QtMcpServer/QMcpServer>
#include "tools.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Qt Mcp Example Window Server");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Signal Slot Inc.");
    app.setOrganizationDomain("signal-slot.co.jp");

    QCommandLineParser parser;
    parser.setApplicationDescription(app.applicationName());
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption backendOption(QStringList() << "b" << "backend",
                                   "Backend to use (stdio/sse).",
                                   "backend", "stdio");
    parser.addOption(backendOption);

    QCommandLineOption addressOption(QStringList() << "a" << "address",
                                   "Address to listen on (host:port).",
                                   "address", "127.0.0.1:8000");
    parser.addOption(addressOption);

    parser.process(app);

    QString backend = parser.value(backendOption);
    QString address = parser.value(addressOption);

    QMcpServer server(backend);
    server.registerToolSet(new Tools(&server)
                           , {
                            { "screenShot", "Take screen shot of whole screen" },
                            { "moveCursor", "Move cursor to specified position" },
                            { "moveCursor/x", "X coordinate of cursor" },
                            { "moveCursor/y", "Y coordinate of cursor" },
                            });
    server.start(address);

    return app.exec();
}
