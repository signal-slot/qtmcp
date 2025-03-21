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
    QCOMPARE(m_session->resourceTemplates().size(), 1);
    QCOMPARE(m_session->resourceTemplates().first().name(), QStringLiteral("test"));
}

void tst_QMcpServerSession::testInsertResourceTemplate()
{
    QMcpResourceTemplate tmpl1, tmpl2;
    tmpl1.setName(QStringLiteral("test1"));
    tmpl2.setName(QStringLiteral("test2"));

    m_session->appendResourceTemplate(tmpl1);
    m_session->insertResourceTemplate(0, tmpl2);
    QCOMPARE(m_session->resourceTemplates().size(), 2);
    QCOMPARE(m_session->resourceTemplates().first().name(), QStringLiteral("test2"));
}

void tst_QMcpServerSession::testReplaceResourceTemplate()
{
    QMcpResourceTemplate tmpl1, tmpl2;
    tmpl1.setName(QStringLiteral("test1"));
    tmpl2.setName(QStringLiteral("test2"));

    m_session->appendResourceTemplate(tmpl1);
    m_session->replaceResourceTemplate(0, tmpl2);
    QCOMPARE(m_session->resourceTemplates().size(), 1);
    QCOMPARE(m_session->resourceTemplates().first().name(), QStringLiteral("test2"));
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
    QCOMPARE(m_session->resources().size(), 1);
    QCOMPARE(m_session->resources().first().name(), QStringLiteral("Test Resource"));
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

    QCOMPARE(m_session->prompts().size(), 1);
    QCOMPARE(m_session->prompts().first().name(), QStringLiteral("test"));
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
    QCOMPARE(m_session->messages(QStringLiteral("test")).size(), 1);
    QCOMPARE(m_session->messages(QStringLiteral("test")).first().content().textContent().text(), QStringLiteral("Test message"));
}

void tst_QMcpServerSession::testTools()
{
    QVERIFY(m_session->tools().isEmpty());
}

void tst_QMcpServerSession::testCallTool()
{
    bool ok = false;
    QJsonObject params;
    params[QStringLiteral("test")] = QStringLiteral("value");

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

    QCOMPARE(m_session->roots().size(), 1);
    QCOMPARE(m_session->roots().first().name(), QStringLiteral("test"));
    QCOMPARE(rootsSpy.count(), 1);
}

QTEST_MAIN(tst_QMcpServerSession)
#include "tst_qmcpserversession.moc"
