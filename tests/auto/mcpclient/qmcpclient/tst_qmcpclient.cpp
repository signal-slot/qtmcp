// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtCore/QProcess>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

#include <QtMcpClient/QMcpClient>
#include <QtMcpCommon>

#include "../../mcpcommon/testhelper.h"

class tst_QMcpClient : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testInitialize();
    void testListTools();
    void testCallTool();

private:
    QMcpInitializeResult initialize();
    static const int SERVER_TIMEOUT = 15000; // 15 seconds
    QMcpClient *m_client = nullptr;
};

void tst_QMcpClient::init()
{
    // Create client with stdio backend
    m_client = new QMcpClient("stdio", this);
    
    QEventLoop loop;
    connect(m_client, &QMcpClient::started, &loop, &QEventLoop::quit);
    // Start the client with server's process ID
    m_client->start("npx @modelcontextprotocol/server-everything"_L1);
    QTimer::singleShot(SERVER_TIMEOUT, &loop, &QEventLoop::quit);
    loop.exec();
}

void tst_QMcpClient::cleanup()
{
    if (m_client) {
        delete m_client;
        m_client = nullptr;
    }
}

QMcpInitializeResult tst_QMcpClient::initialize()
{
    QMcpInitializeResult ret;

    QEventLoop loop;

    // Create initialize request
    QMcpInitializeRequest request;
    auto params = request.params();
    auto clientInfo = params.clientInfo();
    clientInfo.setName("QtMcpClient-Test");
    clientInfo.setVersion("1.0.0");
    params.setClientInfo(clientInfo);
    params.setProtocolVersion(QtMcp::ProtocolVersion::v2025_03_26);

    m_client->request(request, [&](const QMcpInitializeResult &result, const QMcpJSONRPCErrorError *) {
        QMcpInitializedNotification notification;
        m_client->notify(notification);
        ret = result;
        loop.quit();
    });

    // Wait for response with timeout
    QTimer::singleShot(SERVER_TIMEOUT, &loop, &QEventLoop::quit);
    loop.exec();
    return ret;
}

void tst_QMcpClient::testInitialize()
{
    const auto result = initialize();
    QCOMPARE(result.protocolVersion(), QtMcp::ProtocolVersion::v2025_03_26);
}

void tst_QMcpClient::testListTools()
{
    initialize();

    QEventLoop loop;
    bool received = false;

    // Create list tools request
    QMcpListToolsRequest request;
    m_client->request<QMcpListToolsRequest>(request, [&](const QMcpListToolsResult &result, const QMcpJSONRPCErrorError *) {
        QVERIFY(!result.tools().isEmpty());
        received = true;
        loop.quit();
    });

    // Wait for response with timeout
    QTimer::singleShot(SERVER_TIMEOUT, &loop, &QEventLoop::quit);
    loop.exec();

    QVERIFY2(received, "List tools request timed out");
}

void tst_QMcpClient::testCallTool()
{
    initialize();

    QEventLoop loop;
    bool received = false;

    // Create call tool request
    QMcpCallToolRequest request;
    auto params = request.params();
    params.setName("echo");
    QJsonObject args;
    args.insert("message", "Hello MCP!");
    params.setArguments(args);
    request.setParams(params);

    m_client->request<QMcpCallToolRequest>(request, [&](const QMcpCallToolResult &result, const QMcpJSONRPCErrorError *) {
        QVERIFY2(!result.isError(), "Tool call returned an error");
        QVERIFY2(!result.content().isEmpty(), "Tool call returned no content");
        
        // The echo tool should return our message
        const auto contents = result.content();
        const auto content = contents.first();
        QCOMPARE(content.refType(), "textContent");
        QCOMPARE(content.textContent().text(), "Echo: Hello MCP!");
        
        received = true;
        loop.quit();
    });

    // Wait for response with timeout
    QTimer::singleShot(SERVER_TIMEOUT, &loop, &QEventLoop::quit);
    loop.exec();

    QVERIFY2(received, "Call tool request timed out");
}

QTEST_MAIN(tst_QMcpClient)
#include "tst_qmcpclient.moc"
