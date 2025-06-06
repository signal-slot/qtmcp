// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QTemporaryFile>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>

#include <QtMcpClient/QMcpClient>
#include <QtMcpServer/QMcpServer>
#include <QtMcpServer/QMcpServerSession>
#include <QtMcpCommon/QMcpAnnotated>
#include <QtMcpCommon/QMcpPromptMessage>
#include <QtMcpCommon/QMcpInitializeRequest>
#include <QtMcpCommon/QMcpInitializeResult>
#include <QtMcpCommon/qtmcpnamespace.h>

class tst_VersionNegotiation : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // Version negotiation tests
    void testDefaultVersionNegotiation();
    void testSpecificVersionNegotiation_data();
    void testSpecificVersionNegotiation();
    void testUnsupportedVersionFallback();

private:
    QMcpServer *m_server = nullptr;
    QMcpClient *m_client = nullptr;
    QTemporaryFile m_tempFile;

    // Helper methods
    QtMcp::ProtocolVersion connectClientServer();
    QMcpServerSession* getServerSession();
};

void tst_VersionNegotiation::init()
{
    // Set up the MCP server with sse backend
    m_server = new QMcpServer("sse", this);

    // Set supported protocol versions
    m_server->setSupportedProtocolVersions({QtMcp::ProtocolVersion::v2025_03_26, QtMcp::ProtocolVersion::v2024_11_05});
    m_server->setProtocolVersion(QtMcp::ProtocolVersion::v2025_03_26); // Default to latest version

    // Start the server with specific address
    m_server->start("127.0.0.1:10101");

    // Set up the MCP client with sse backend
    m_client = new QMcpClient("sse", this);

    // Start the client - connect to the server using the same address
    m_client->start("http://localhost:10101");
}

void tst_VersionNegotiation::cleanup()
{
    if (m_client) {
        delete m_client;
        m_client = nullptr;
    }

    if (m_server) {
        delete m_server;
        m_server = nullptr;
    }
}

QtMcp::ProtocolVersion tst_VersionNegotiation::connectClientServer()
{
    QSignalSpy startedSpy(m_client, &QMcpClient::started);

    if (!startedSpy.wait(5000))
        return QtMcp::ProtocolVersion::Latest;

    // Send initialization request with properly set protocol version
    QMcpInitializeRequest request;
    QMcpInitializeRequestParams params = request.params();

    // Set the protocol version in the params - convert enum to string since params still uses strings
    params.setProtocolVersion(QtMcp::protocolVersionToString(m_client->protocolVersion()));

    // Set the params back to the request
    request.setParams(params);

    QEventLoop loop;

    // Set up a flag to track initialization success
    QtMcp::ProtocolVersion protocolVersion;

    m_client->request(request, [&protocolVersion, &loop](const QMcpInitializeResult &result, const QMcpJSONRPCErrorError *error) {
        // Handle the response in the callback
        if (error) {
            qDebug() << "Initialization failed:" << error->message();
        } else {
            protocolVersion = result.protocolVersion();
        }
        loop.quit();
    });

    QTimer::singleShot(5000, &loop, &QEventLoop::quit);
    loop.exec();

    return protocolVersion;
}

QMcpServerSession* tst_VersionNegotiation::getServerSession()
{
    const auto sessions = m_server->sessions();
    if (sessions.isEmpty())
        return nullptr;

    return sessions.first();
}

void tst_VersionNegotiation::testDefaultVersionNegotiation()
{
    const auto clientProtocolVersion = connectClientServer();

    // Check that both client and server negotiated to the latest version
    QMcpServerSession *session = getServerSession();
    QVERIFY(session);

    // Both should be using the latest protocol version
    QCOMPARE(session->protocolVersion(), clientProtocolVersion);
}

void tst_VersionNegotiation::testSpecificVersionNegotiation_data()
{
    QTest::addColumn<QtMcp::ProtocolVersion>("requestedVersion");
    QTest::addColumn<QtMcp::ProtocolVersion>("expectedVersion");

    QTest::newRow("Latest version") << QtMcp::ProtocolVersion::v2025_03_26 << QtMcp::ProtocolVersion::v2025_03_26;
    QTest::newRow("Old version") << QtMcp::ProtocolVersion::v2024_11_05 << QtMcp::ProtocolVersion::v2024_11_05;
}

void tst_VersionNegotiation::testSpecificVersionNegotiation()
{
    QFETCH(QtMcp::ProtocolVersion, requestedVersion);
    QFETCH(QtMcp::ProtocolVersion, expectedVersion);

    // Set the protocol version explicitly before sending request
    m_client->setProtocolVersion(requestedVersion);

    const auto clientProtocolVersion = connectClientServer();
    QCOMPARE(clientProtocolVersion, expectedVersion);

    // Verify the server session is using the expected protocol version
    QMcpServerSession *session = getServerSession();
    QVERIFY(session);
    QCOMPARE(session->protocolVersion(), expectedVersion);
}

void tst_VersionNegotiation::testUnsupportedVersionFallback()
{
    // Try to set an unsupported protocol version
    // This should use an invalid enum value instead
    // For now, we'll convert a string that will map to an invalid or default enum value
    m_client->setProtocolVersion(QtMcp::stringToProtocolVersion("9999-99-99"));

    const auto clientProtocolVersion = connectClientServer();

    // Server should fall back to the latest supported version
    QCOMPARE(clientProtocolVersion, QtMcp::ProtocolVersion::v2025_03_26);

    // Verify the server session uses the latest supported version as fallback
    QMcpServerSession *session = getServerSession();
    QVERIFY(session);
    QCOMPARE(session->protocolVersion(), clientProtocolVersion);
}

QTEST_MAIN(tst_VersionNegotiation)
#include "tst_version_negotiation.moc"
