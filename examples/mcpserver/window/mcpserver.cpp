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
    // initialize
    q->addRequestHandler([](const QMcpInitializeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpInitializeResult result;
        if (request.params().protocolVersion() == "2024-11-05") {
            auto serverInfo = result.serverInfo();
            serverInfo.setName("Qt MCP window server");
            serverInfo.setVersion("0.1.0");
            result.setServerInfo(serverInfo);
            result.setProtocolVersion("2024-11-05");
        } else {
            error->setCode(20241105);
            error->setMessage("Protocol Version %1 is not supported"_L1.arg(request.params().protocolVersion()));
        }
        return result;
    });
    q->addNotificationHandler([this](const QMcpInitializedNotification &) {
        initialized = true;
    });

    // ping
    q->addRequestHandler([](const QMcpPingRequest &, QMcpJSONRPCErrorError *) {
        QMcpResult result;
        return result;
    });

    // tools
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
        const auto content = q->callTool(params.name(), params.arguments(), &ok);
        if (ok) {
            auto contents = result.content();
            contents.append(content);
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

QImage McpServer::screenShot() const
{
    return QGuiApplication::primaryScreen()->grabWindow().toImage();
}

void McpServer::moveCursor(int x, int y)
{
    QCursor::setPos(x, y);
}
