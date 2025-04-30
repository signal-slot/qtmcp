// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include <QtMcpCommon/QMcpNotification>
#include <QtMcpCommon/QMcpRequest>
#include <QtMcpCommon/QMcpResult>
#include <QtMcpServer/QMcpServer>
#include <QtTest/QTest>

QT_BEGIN_NAMESPACE

class EchoRequest : public QMcpRequest
{
    Q_GADGET
public:
    EchoRequest() : QMcpRequest(new Private) {}
    QString method() const override { return QStringLiteral("test.echo"); }
    QString message;

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpRequest::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

class EchoResult : public QMcpResult
{
    Q_GADGET
public:
    EchoResult() : QMcpResult(new Private) {}
    QString message;

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }
protected:
    struct Private : public QMcpResult::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

class TestNotification : public QMcpNotification
{
    Q_GADGET
public:
    TestNotification() : QMcpNotification(new Private) {}
    QString method() const override { return QStringLiteral("test.notification"); }
    QString message;

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }
protected:
    struct Private : public QMcpNotification::Private {
        Private *clone() const override { return new Private(*this); }
    };
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
    m_server->addRequestHandler([](const QUuid &, const EchoRequest &request, QMcpJSONRPCErrorError *) -> EchoResult {
        EchoResult result;
        result.message = QStringLiteral("Echo: ") + request.message;
        return result;
    });

    // Send a request through the server
    EchoRequest request;
    request.message = QStringLiteral("Hello MCP!");

    connect(m_server, &QMcpServer::result, this, [&](const QUuid &, const QJsonObject &resultJson) {
        EchoResult result;
        result.fromJsonObject(resultJson);
        QCOMPARE(result.message, QStringLiteral("Echo: Hello MCP!"));
    });

    m_server->request(QUuid::createUuid(), request);
}

void tst_QMcpServer::testNotificationHandler()
{
    m_server->addNotificationHandler([](const QUuid &, const TestNotification &notification) {
        QCOMPARE(notification.message, QStringLiteral("Test notification"));
    });

    // Send a notification
    TestNotification notification;
    notification.message = QStringLiteral("Test notification");
    m_server->notify(QUuid(), notification);
}

QTEST_MAIN(tst_QMcpServer)
#include "tst_qmcpserver.moc"
