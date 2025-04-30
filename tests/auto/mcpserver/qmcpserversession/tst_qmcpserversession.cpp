// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QEventLoop>
#include <QtCore/QJsonObject>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QUuid>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include <QtMcpCommon/QMcpResourceTemplate>
#include <QtMcpCommon/QMcpResource>
#include <QtMcpCommon/QMcpPrompt>
#include <QtMcpCommon/QMcpTool>
#include <QtMcpCommon/QMcpRoot>
#include <QtMcpCommon/QMcpPromptMessage>
#include <QtMcpCommon/QMcpReadResourceResultContents>
#include <QtMcpCommon/QMcpAnnotated>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtMcpServer/QMcpServer>
#include <QtMcpServer/QMcpServerSession>

class tst_QMcpServerSession : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // Basic properties
    void testSessionId();
    void testInitialization();

    // Protocol version handling
    void testProtocolVersion();
    void testProtocolVersionValidation_data();
    void testProtocolVersionValidation();
    void testAnnotationsWithVersion();

    // Resource template management
    void testResourceTemplates();
    void testAppendResourceTemplate();
    void testInsertResourceTemplate();
    void testReplaceResourceTemplate();
    void testRemoveResourceTemplate();

    // Resource management
    void testResources();
    void testResourceOperations();
    void testResourceSubscription();

    // Prompt management
    void testPrompts();
    void testPromptOperations();
    void testMessages();

    // Tool management
    void testTools();
    void testCallTool();

    // Root management
    void testRoots();

private:
    static const int TIMEOUT = 1000; // 1 second
    QMcpServerSession *m_session = nullptr;
    QUuid m_sessionId;
};

void tst_QMcpServerSession::init()
{
    m_sessionId = QUuid::createUuid();
    m_session = new QMcpServerSession(m_sessionId);
}

void tst_QMcpServerSession::cleanup()
{
    if (m_session) {
        delete m_session;
        m_session = nullptr;
    }
}

void tst_QMcpServerSession::testSessionId()
{
    QCOMPARE(m_session->sessionId(), m_sessionId);
}

void tst_QMcpServerSession::testInitialization()
{
    QVERIFY(!m_session->isInitialized());

    QSignalSpy spy(m_session, &QMcpServerSession::initializedChanged);
    m_session->setInitialized(true);

    QVERIFY(m_session->isInitialized());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().first().toBool(), true);
}

void tst_QMcpServerSession::testProtocolVersion()
{
    // Check default protocol version
    QCOMPARE(m_session->protocolVersion(), QtMcp::ProtocolVersion::Latest);

    // Check protocol version setter and getter
    m_session->setProtocolVersion(QtMcp::ProtocolVersion::v2024_11_05);
    QCOMPARE(m_session->protocolVersion(), QtMcp::ProtocolVersion::v2024_11_05);

    // Change back to default version
    m_session->setProtocolVersion(QtMcp::ProtocolVersion::v2025_03_26);
    QCOMPARE(m_session->protocolVersion(), QtMcp::ProtocolVersion::v2025_03_26);
}

void tst_QMcpServerSession::testProtocolVersionValidation_data()
{
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("2025-03-26") << "2025-03-26" << true;
    QTest::newRow("2024-11-05") << "2024-11-05" << true;
    QTest::newRow("invalid date format") << "2025/03/26" << false;
    QTest::newRow("future version") << "2026-01-01" << false;
    QTest::newRow("empty") << "" << false;
}

void tst_QMcpServerSession::testProtocolVersionValidation()
{
    QFETCH(QString, version);
    QFETCH(bool, isValid);

    if (isValid) {
        // Since we now have the string-based overload for backward compatibility
        QtMcp::ProtocolVersion expectedVersion = QtMcp::stringToProtocolVersion(version);
        m_session->setProtocolVersion(version);
        QCOMPARE(m_session->protocolVersion(), expectedVersion);
    } else {
        // Should remain at the default version since invalid version is rejected
        QtMcp::ProtocolVersion originalVersion = m_session->protocolVersion();
        m_session->setProtocolVersion(version);
        QCOMPARE(m_session->protocolVersion(), originalVersion);
    }
}

void tst_QMcpServerSession::testAnnotationsWithVersion()
{
    // Create an annotated object
    QMcpAnnotated annotated;
    QMcpAnnotations annotations;
    annotations.setAudience({QMcpRole::assistant});
    annotations.setPriority(0.8);
    annotated.setAnnotations(annotations);

    // Test with 2025-03-26 version (should include annotations)
    m_session->setProtocolVersion(QtMcp::ProtocolVersion::v2025_03_26);
    QJsonObject jsonObj2025 = annotated.toJsonObject(m_session->protocolVersion());
    QVERIFY(jsonObj2025.contains("annotations"));
    QVERIFY(jsonObj2025.value("annotations").isObject());
    QVERIFY(jsonObj2025.value("annotations").toObject().contains("audience"));
    QVERIFY(jsonObj2025.value("annotations").toObject().contains("priority"));

    // Test with 2024-11-05 version (should not include annotations)
    m_session->setProtocolVersion(QtMcp::ProtocolVersion::v2024_11_05);
    QJsonObject jsonObj2024 = annotated.toJsonObject(m_session->protocolVersion());
    QVERIFY(!jsonObj2024.contains("annotations"));
}

void tst_QMcpServerSession::testResourceTemplates()
{
    QVERIFY(m_session->resourceTemplates().isEmpty());
}

void tst_QMcpServerSession::testAppendResourceTemplate()
{
    QMcpResourceTemplate tmpl;
    tmpl.setName(QStringLiteral("test"));
    tmpl.setUriTemplate(QStringLiteral("test://{param}"));

    m_session->appendResourceTemplate(tmpl);
    const auto resourceTemplates = m_session->resourceTemplates();
    QCOMPARE(resourceTemplates.size(), 1);
    QCOMPARE(resourceTemplates.first().name(), QStringLiteral("test"));
}

void tst_QMcpServerSession::testInsertResourceTemplate()
{
    QMcpResourceTemplate tmpl1, tmpl2;
    tmpl1.setName(QStringLiteral("test1"));
    tmpl2.setName(QStringLiteral("test2"));

    m_session->appendResourceTemplate(tmpl1);
    m_session->insertResourceTemplate(0, tmpl2);
    const auto resourceTemplates = m_session->resourceTemplates();
    QCOMPARE(resourceTemplates.size(), 2);
    QCOMPARE(resourceTemplates.first().name(), QStringLiteral("test2"));
}

void tst_QMcpServerSession::testReplaceResourceTemplate()
{
    QMcpResourceTemplate tmpl1, tmpl2;
    tmpl1.setName(QStringLiteral("test1"));
    tmpl2.setName(QStringLiteral("test2"));

    m_session->appendResourceTemplate(tmpl1);
    m_session->replaceResourceTemplate(0, tmpl2);
    const auto resourceTemplates = m_session->resourceTemplates();
    QCOMPARE(resourceTemplates.size(), 1);
    QCOMPARE(resourceTemplates.first().name(), QStringLiteral("test2"));
}

void tst_QMcpServerSession::testRemoveResourceTemplate()
{
    QMcpResourceTemplate tmpl;
    tmpl.setName(QStringLiteral("test"));

    m_session->appendResourceTemplate(tmpl);
    m_session->removeResourceTemplateAt(0);
    QVERIFY(m_session->resourceTemplates().isEmpty());
}

void tst_QMcpServerSession::testResources()
{
    QVERIFY(m_session->resources().isEmpty());
}

void tst_QMcpServerSession::testResourceOperations()
{
    QMcpResource resource;
    resource.setUri(QUrl(QStringLiteral("test://resource")));
    resource.setName(QStringLiteral("Test Resource"));

    QMcpTextResourceContents textContent;
    textContent.setMimeType(QStringLiteral("text/plain"));
    textContent.setText(QStringLiteral("Test content"));
    QMcpReadResourceResultContents content(textContent);

    QSignalSpy resourceListSpy(m_session, &QMcpServerSession::resourceListChanged);
    m_session->appendResource(resource, content);

    QTest::qWait(10);
    const auto resources = m_session->resources();
    QCOMPARE(resources.size(), 1);
    QCOMPARE(resources.first().name(), QStringLiteral("Test Resource"));
    QCOMPARE(m_session->contents(resource.uri()).first().textResourceContents().text(), QStringLiteral("Test content"));
    QCOMPARE(resourceListSpy.count(), 1);
}

void tst_QMcpServerSession::testResourceSubscription()
{
    QUrl uri(QStringLiteral("test://resource"));
    QVERIFY(!m_session->isSubscribed(uri));

    m_session->subscribe(uri);
    QVERIFY(m_session->isSubscribed(uri));

    m_session->unsubscribe(uri);
    QVERIFY(!m_session->isSubscribed(uri));
}

void tst_QMcpServerSession::testPrompts()
{
    QVERIFY(m_session->prompts().isEmpty());
}

void tst_QMcpServerSession::testPromptOperations()
{
    QMcpPrompt prompt;
    prompt.setName(QStringLiteral("test"));

    QMcpPromptMessage message;
    message.setRole(QMcpRole::user);
    message.setContent(QMcpTextContent("Test message"_L1));

    QSignalSpy promptListSpy(m_session, &QMcpServerSession::promptListChanged);
    m_session->appendPrompt(prompt, message);
    QTest::qWait(10);

    const auto prompts = m_session->prompts();
    QCOMPARE(prompts.size(), 1);
    QCOMPARE(prompts.first().name(), QStringLiteral("test"));
    QCOMPARE(promptListSpy.count(), 1);
}

void tst_QMcpServerSession::testMessages()
{
    QMcpPrompt prompt;
    prompt.setName(QStringLiteral("test"));

    QMcpPromptMessage message;
    message.setRole(QMcpRole::user);
    message.setContent(QMcpTextContent("Test message"_L1));

    m_session->appendPrompt(prompt, message);
    const auto messages = m_session->messages(QStringLiteral("test"));
    QCOMPARE(messages.size(), 1);
    QCOMPARE(messages.first().content().textContent().text(), QStringLiteral("Test message"));
}

void tst_QMcpServerSession::testTools()
{
    QVERIFY(m_session->tools().isEmpty());
}

void tst_QMcpServerSession::testCallTool()
{
    bool ok = false;
    QJsonObject params;
    params.insert(QStringLiteral("test"), QStringLiteral("value"));

    auto result = m_session->callTool(QStringLiteral("test"), params, &ok);
    QVERIFY(!ok); // Should fail as no tools are registered
    QVERIFY(result.isEmpty());
}

void tst_QMcpServerSession::testRoots()
{
    QVERIFY(m_session->roots().isEmpty());

    QList<QMcpRoot> roots;
    QMcpRoot root;
    root.setName(QStringLiteral("test"));
    roots.append(root);

    QSignalSpy rootsSpy(m_session, &QMcpServerSession::rootsChanged);
    m_session->setRoots(roots);

    roots = m_session->roots();
    QCOMPARE(roots.size(), 1);
    QCOMPARE(roots.first().name(), QStringLiteral("test"));
    QCOMPARE(rootsSpy.count(), 1);
}

QTEST_MAIN(tst_QMcpServerSession)
#include "tst_qmcpserversession.moc"
