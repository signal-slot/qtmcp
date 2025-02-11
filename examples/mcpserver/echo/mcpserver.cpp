// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "mcpserver.h"
#include <QtMcpCommon/QtMcpCommon>
#include <QtCore/QHash>
#include <QtCore/QUuid>

class McpServer::Private
{
public:
    Private(McpServer *parent);

private:
    McpServer *q;
    QHash<QUuid, bool> initialized;
};

McpServer::Private::Private(McpServer *parent)
    : q(parent)
{
    connect(q, &McpServer::newSessionStarted, q, [this](const QUuid &session) {
        initialized.insert(session, false);
    });

    q->addRequestHandler([this](const QUuid &session, const QMcpInitializeRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpInitializeResult result;
        if (initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Initialized"_L1);
            return result;
        }
        if (request.params().protocolVersion() != "2024-11-05") {
            error->setCode(20241105);
            error->setMessage("Protocol Version %1 is not supported"_L1.arg(request.params().protocolVersion()));
            return result;
        }
        auto serverInfo = result.serverInfo();
        serverInfo.setName("Qt MCP echo server");
        serverInfo.setVersion("0.1.0");
        result.setServerInfo(serverInfo);
        result.setProtocolVersion("2024-11-05");
        return result;
    });
    q->addNotificationHandler([this](const QUuid &session, const QMcpInitializedNotification &notification) {
        initialized[session] = true;
    });

    q->addRequestHandler([](const QUuid &session, const QMcpPingRequest &, QMcpJSONRPCErrorError *) {
        Q_UNUSED(session); // ping can be accepted even before initialization
        QMcpResult result;
        return result;
    });

    q->addRequestHandler([this](const QUuid &session, const QMcpListToolsRequest &, QMcpJSONRPCErrorError *error) {
        QMcpListToolsResult result;
        if (!initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }
        result.setTools(q->tools());
        return result;
    });

    q->addRequestHandler([this](const QUuid &session, const QMcpCallToolRequest &request, QMcpJSONRPCErrorError *error) {
        QMcpCallToolResult result;
        if (!initialized.value(session, false)) {
            error->setCode(1);
            error->setMessage("Not initialized"_L1);
            return result;
        }

        const auto params = request.params();
        bool ok;
        auto contents = q->callTool(session, params.name(), params.arguments(), &ok);
        if (ok) {
            result.setContent(contents);
        }
        return result;
    });
}

McpServer::McpServer(const QString &backend, QObject *parent)
    : QMcpServer(backend, parent)
    , d(new Private(this))
{}

McpServer::~McpServer() = default;

QString McpServer::echo(const QString &message, const QUuid &session) const
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
