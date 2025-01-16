// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QString>
#include <QtCore/QByteArray>

#include <QtMcpServer/qmcpserver.h>
#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcptool.h>
#include <QtMcpCommon/qmcpjsonrpcerrorerror.h>

#include "../../mcpcommon/testhelper.h"

QT_BEGIN_NAMESPACE

class EchoRequest : public QMcpRequest
{
public:
    QString method() const override { return QStringLiteral("test.echo"); }
    QString message;

    void fromJsonObject(const QJsonObject &json) override
    {
        QMcpRequest::fromJsonObject(json);
        const auto params = json[QStringLiteral("params")].toObject();
        message = params[QStringLiteral("message")].toString();
    }

    QJsonObject toJsonObject() const override
    {
        QJsonObject json = QMcpRequest::toJsonObject();
        QJsonObject params;
        params[QStringLiteral("message")] = message;
        json[QStringLiteral("params")] = params;
        return json;
    }
};

class EchoResult : public QMcpResult
{
public:
    QString message;

    void fromJsonObject(const QJsonObject &json) override
    {
        QMcpResult::fromJsonObject(json);
        const auto result = json[QStringLiteral("result")].toObject();
        message = result[QStringLiteral("message")].toString();
    }

    QJsonObject toJsonObject() const override
    {
        QJsonObject json = QMcpResult::toJsonObject();
        QJsonObject result;
        result[QStringLiteral("message")] = message;
        json[QStringLiteral("result")] = result;
        return json;
    }
};

class TestNotification : public QMcpNotification
{
public:
    QString method() const override { return QStringLiteral("test.notification"); }
    QString message;

    void fromJsonObject(const QJsonObject &json) override
    {
        QMcpNotification::fromJsonObject(json);
        const auto params = json[QStringLiteral("params")].toObject();
        message = params[QStringLiteral("message")].toString();
    }

    QJsonObject toJsonObject() const override
    {
        QJsonObject json = QMcpNotification::toJsonObject();
        QJsonObject params;
        params[QStringLiteral("message")] = message;
        json[QStringLiteral("params")] = params;
        return json;
    }
};

QT_END_NAMESPACE

class tst_QMcpServer : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testBasicServer();
    void testRequestHandler();
    void testNotificationHandler();
    void testToolHandling();
    void testSignals();

private:
    static const int TIMEOUT = 1000; // 1 second
    QMcpServer *m_server = nullptr;
};

void tst_QMcpServer::init()
{
    m_server = new QMcpServer(QStringLiteral("stdio"), this);
    
    QEventLoop loop;
    connect(m_server, &QMcpServer::started, &loop, &QEventLoop::quit);
    m_server->start();
    QTimer::singleShot(TIMEOUT, &loop, &QEventLoop::quit);
    loop.exec();
}

void tst_QMcpServer::cleanup()
{
    if (m_server) {
        delete m_server;
        m_server = nullptr;
    }
}

void tst_QMcpServer::testBasicServer()
{
    QVERIFY(m_server != nullptr);
    QVERIFY(!m_server->backends().isEmpty());
}

void tst_QMcpServer::testRequestHandler()
{
    QEventLoop loop;
    bool handlerCalled = false;

    m_server->addRequestHandler([&handlerCalled](const EchoRequest &request, QMcpJSONRPCErrorError *) -> EchoResult {
        handlerCalled = true;
        EchoResult result;
        result.message = QStringLiteral("Echo: ") + request.message;
        return result;
    });

    // Send a request through the server
    EchoRequest request;
    request.message = QStringLiteral("Hello MCP!");
    
    connect(m_server, &QMcpServer::result, this, [&](const QJsonObject &resultJson) {
        EchoResult result;
        result.fromJsonObject(resultJson);
        QCOMPARE(result.message, QStringLiteral("Echo: Hello MCP!"));
        loop.quit();
    });

    m_server->request(request);
    QTimer::singleShot(TIMEOUT, &loop, &QEventLoop::quit);
    loop.exec();

    QVERIFY2(handlerCalled, "Request handler was not called");
}

void tst_QMcpServer::testNotificationHandler()
{
    QEventLoop loop;
    bool handlerCalled = false;

    m_server->addNotificationHandler([&handlerCalled](const TestNotification &notification) {
        handlerCalled = true;
        QCOMPARE(notification.message, QStringLiteral("Test notification"));
        loop.quit();
    });

    // Send a notification
    TestNotification notification;
    notification.message = QStringLiteral("Test notification");
    m_server->notify(notification);

    QTimer::singleShot(TIMEOUT, &loop, &QEventLoop::quit);
    loop.exec();

    QVERIFY2(handlerCalled, "Notification handler was not called");
}

void tst_QMcpServer::testToolHandling()
{
    // Register a tool
    QMcpTool tool;
    tool.setName(QStringLiteral("test-tool"));
    tool.setDescription(QStringLiteral("A test tool"));
    
    QJsonObject schema;
    schema[QStringLiteral("type")] = QStringLiteral("object");
    QJsonObject properties;
    QJsonObject messageProps;
    messageProps[QStringLiteral("type")] = QStringLiteral("string");
    properties[QStringLiteral("message")] = messageProps;
    schema[QStringLiteral("properties")] = properties;
    tool.setInputSchema(schema);

    // Test tool registration
    auto tools = m_server->tools();
    QCOMPARE(tools.size(), 1);
    QCOMPARE(tools.first().name(), QStringLiteral("test-tool"));

    // Test tool calling
    bool ok = false;
    QJsonObject params;
    params[QStringLiteral("message")] = QStringLiteral("Hello from tool!");
    auto result = m_server->callTool(QStringLiteral("test-tool"), params, &ok);
    
    QVERIFY(ok);
    QVERIFY(!result.isEmpty());
    QCOMPARE(result.first().textContent().text(), QStringLiteral("Hello from tool!"));
}

void tst_QMcpServer::testSignals()
{
    QEventLoop loop;
    bool receivedSignal = false;
    bool resultSignal = false;

    connect(m_server, &QMcpServer::received, this, [&](const QJsonObject &) {
        receivedSignal = true;
    });

    connect(m_server, &QMcpServer::result, this, [&](const QJsonObject &) {
        resultSignal = true;
        loop.quit();
    });

    // Send a request that should trigger both signals
    EchoRequest request;
    request.message = QStringLiteral("Signal test");
    m_server->request(request);

    QTimer::singleShot(TIMEOUT, &loop, &QEventLoop::quit);
    loop.exec();

    QVERIFY2(receivedSignal, "Received signal was not emitted");
    QVERIFY2(resultSignal, "Result signal was not emitted");
}

QTEST_MAIN(tst_QMcpServer)
#include "tst_qmcpserver.moc"