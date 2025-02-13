// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtMcpServer/QMcpServer>

class McpServer : public QMcpServer
{
    Q_OBJECT
public:
    explicit McpServer(const QString &backend = "stdio", QObject *parent = nullptr)
        : QMcpServer(backend, parent) {}

    Q_INVOKABLE QString echo(const QString &message) const {
        return message;
    }

    QHash<QString, QString> toolDescriptions() const override {
        return {  { "echo"_L1, "Echoes back the input"_L1 }
                , { "echo/message"_L1, "Message to echo"_L1 } };
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("Qt Mcp Example Echo Server");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Signal Slot Inc.");
    app.setOrganizationDomain("signal-slot.co.jp");

    QCommandLineParser parser;
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

    const QString backend = parser.value(backendOption);
    const QString address = parser.value(addressOption);

    McpServer server(backend);
    server.start(address);

    return app.exec();
}

#include "main.moc"
