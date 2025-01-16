// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QCoreApplication>
#include "mcpserver.h"

int main(int argc, char *argv[])
{
    qSetMessagePattern("[%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{function}:%{line} - %{message}");
    // QLoggingCategory::setFilterRules("qt.psdcore.effectslayer.debug=true");

    QCoreApplication::setOrganizationName("Signal Slot Inc.");
    QCoreApplication::setOrganizationDomain("signal-slot.co.jp");
    QCoreApplication::setApplicationName("Mcp Example Server");

    QCoreApplication app(argc, argv);

    McpServer server;
    server.start();

    return app.exec();
}

