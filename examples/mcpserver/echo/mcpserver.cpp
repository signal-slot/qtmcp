// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "mcpserver.h"
#include <QtMcpCommon/QtMcpCommon>

class McpServer::Private
{
public:
    Private(McpServer *parent);

private:
    McpServer *q;
    bool initialized = false;
};

McpServer::Private::Private(McpServer *parent)
    : q(parent)
{
    q->addRequestHandler([](const QMcpInitializeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpInitializeResult result;
        if (request.params().protocolVersion() == "2024-11-05") {
            auto serverInfo = result.serverInfo();
            serverInfo.setName("Qt MCP echo server");
            serverInfo.setVersion("0.1.0");
            result.setServerInfo(serverInfo);
            result.setProtocolVersion("2024-11-05");
        } else {
            error->setCode(20241105);
            error->setMessage("Protocol Version %1 is not supported"_L1.arg(request.params().protocolVersion()));
        }
        return result;
    });
    q->addNotificationHandler([this](const QMcpInitializedNotification &notification) {
        qDebug() << notification;
        initialized = true;
    });

    q->addRequestHandler([](const QMcpPingRequest &, QMcpJSONRPCErrorError *) {
        QMcpResult result;
        return result;
    });

    q->addRequestHandler([this](const QMcpListToolsRequest &, QMcpJSONRPCErrorError *error) {
        QMcpListToolsResult result;
        if (!initialized) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }
        result.setTools(q->tools());
        return result;
    });

    q->addRequestHandler([this](const QMcpCallToolRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpCallToolResult result;
        if (!initialized) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }

        const auto params = request.params();
        bool ok;
        auto contents = q->callTool(params.name(), params.arguments(), &ok);
        if (ok) {
            result.setContent(contents);
        }
        return result;
    });
}

McpServer::McpServer(QObject *parent)
    : QMcpServer("stdio"_L1, parent)
    , d(new Private(this))
{}

McpServer::~McpServer() = default;

QString McpServer::echo(const QString &message) const
{
    return message;
}

QHash<QString, QString> McpServer::descriptions() const
{
    return {
        { "echo"_L1, "Echoes back the input"_L1 },
        { "echo/message"_L1, "Message to echo"_L1 }
    };
}
