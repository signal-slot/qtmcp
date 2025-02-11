// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore>
#include "mcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Mcp Echo Server");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Signal Slot Inc.");
    app.setOrganizationDomain("signal-slot.co.jp");

    QCommandLineParser parser;
    parser.setApplicationDescription("MCP Echo Server Example");
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

    McpServer server(backend);
    server.start(address);

    return app.exec();
}
