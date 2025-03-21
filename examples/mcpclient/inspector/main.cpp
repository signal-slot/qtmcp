// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    qSetMessagePattern("[%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{function}:%{line} - %{message}");
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Signal Slot Inc.");
    QCoreApplication::setOrganizationDomain("signal-slot.co.jp");
    QCoreApplication::setApplicationName("MCP Inspector");

    MainWindow window;
    window.show();

    return app.exec();
}
