// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERSESSION_H
#define QMCPSERVERSESSION_H

#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QUuid>
#include <QtMcpCommon/QMcpCallToolResultContent>
#include <QtMcpCommon/QMcpCreateMessageRequestParams>
#include <QtMcpCommon/QMcpCreateMessageResult>
#include <QtMcpCommon/QMcpPrompt>
#include <QtMcpCommon/QMcpPromptMessage>
#include <QtMcpCommon/QMcpReadResourceResultContents>
#include <QtMcpCommon/QMcpResource>
#include <QtMcpCommon/QMcpResourceTemplate>
#include <QtMcpCommon/QMcpRoot>
#include <QtMcpCommon/QMcpTool>
#include <QtMcpServer/qmcpserverglobal.h>

QT_BEGIN_NAMESPACE

#ifdef QT_GUI_LIB
class QAction;
#endif

class QMcpServer;

/*!
    \class QMcpServerSession
    \inmodule QtMcpServer
    \brief The QMcpServerSession class represents a client session in an MCP server.

    QMcpServerSession manages the state and interactions for a single client connection,
    including:
    \list
    \li Resource management (templates, instances, and subscriptions)
    \li Prompt handling and message history
    \li Tool execution
    \li Root management
    \endlist

    Each session is identified by a unique UUID and maintains its own state independently
    of other sessions.
*/
class Q_MCPSERVER_EXPORT QMcpServerSession : public QObject
{
    Q_OBJECT
    /*!
        \property QMcpServerSession::initialized
        \brief Whether the session has been initialized.

        A session becomes initialized after the client has successfully completed
        the initialization handshake.
    */
    Q_PROPERTY(bool initialized READ isInitialized WRITE setInitialized NOTIFY initializedChanged FINAL)
public:
    /*!
        Constructs a server session with the given ID and parent.
        \param sessionId Unique identifier for this session
        \param parent The parent QMcpServer object
    */
    explicit QMcpServerSession(const QUuid &sessionId, QMcpServer *parent = nullptr);

    /*!
        Destroys the server session.
    */
    ~QMcpServerSession() override;

    /*!
        Returns the unique identifier for this session.
    */
    QUuid sessionId() const;

    /*!
        Returns whether the session has been initialized.
        \sa initialized
    */
    bool isInitialized() const;

    /*!
        Sets whether the session has been initialized.
        \param initialized The new initialization state
        \sa initialized
    */
    void setInitialized(bool initialized);

    /*!
        Returns the list of resource templates available in this session.
    */
    QList<QMcpResourceTemplate> resourceTemplates() const;

    // Resource management
    /*!
        Returns whether this session is subscribed to the resource at the given URI.
        \param uri The resource URI to check
        \return true if subscribed, false otherwise
    */
    bool isSubscribed(const QUrl &uri) const;

    /*!
        Returns the list of resources available in this session.
        \param cursor Optional cursor for pagination
        \return List of resources
     */
    QList<QMcpResource> resources(QString *cursor = nullptr) const;

    /*!
        Returns the contents of the resource at the given URI.
        \param uri The resource URI to get contents for
        \return List of resource contents
     */
    QList<QMcpReadResourceResultContents> contents(const QUrl &uri) const;

    // Prompt management
    /*!
        Returns the list of prompts in this session.
        \param cursor Optional cursor for pagination
        \return List of prompts
     */
    QList<QMcpPrompt> prompts(QString *cursor = nullptr) const;

    /*!
        Returns the message history for a prompt.
        \param name Name of the prompt
        \return List of prompt messages
     */
    QList<QMcpPromptMessage> messages(const QString &name) const;

    // Tool management
    /*!
        Returns the list of tools in this session.
        \param cursor Optional cursor for pagination
        \return List of tools
     */
    QList<QMcpTool> tools(QString *cursor = nullptr) const;

    /*!
        Executes a tool with the given parameters.
        \param name Name of the tool to execute
        \param params Parameters for the tool
        \param ok Optional pointer to bool that will be set to true if successful
        \return List of tool execution results
     */
    QList<QMcpCallToolResultContent> callTool(const QString &name, const QJsonObject &params, bool *ok = nullptr);

    /*!
        Returns the list of roots available in this session.
        \param cursor Optional cursor for pagination
        \return List of roots
     */
    QList<QMcpRoot> roots(QString *cursor = nullptr) const;

public slots:
    /*!
        Appends a resource template to the session.
        \param resourceTemplate The template to append
    */
    void appendResourceTemplate(const QMcpResourceTemplate &resourceTemplate);

    /*!
        Inserts a resource template at the specified index.
        \param index Position to insert at
        \param resourceTemplate The template to insert
    */
    void insertResourceTemplate(int index, const QMcpResourceTemplate &resourceTemplate);

    /*!
        Replaces the resource template at the specified index.
        \param index Position to replace at
        \param resourceTemplate The new template
    */
    void replaceResourceTemplate(int index, const QMcpResourceTemplate resourceTemplate);

    /*!
        Removes the resource template at the specified index.
        \param index Position to remove from
    */
    void removeResourceTemplateAt(int index);

    void appendResource(const QMcpResource &resource, const QMcpReadResourceResultContents &content);
    void insertResource(int index, const QMcpResource &resource, const QMcpReadResourceResultContents &content);
    void replaceResource(const QUrl &uri, const QMcpResource resource, const QMcpReadResourceResultContents &content);
    void replaceResource(int index, const QMcpResource resource, const QMcpReadResourceResultContents &content);
    void removeResource(const QUrl &uri);
    void removeResourceAt(int index);

    void appendPrompt(const QMcpPrompt &prompt, const QMcpPromptMessage &message);
    void insertPrompt(int index, const QMcpPrompt &prompt, const QMcpPromptMessage &message);
    void replacePrompt(int index, const QMcpPrompt prompt, const QMcpPromptMessage &message);
    void removePromptAt(int index);

    void subscribe(const QUrl &uri);
    void unsubscribe(const QUrl &uri);

    void registerToolSet(QObject *toolSet, const QHash<QString, QString> &descriptions = {});
    void unregisterToolSet(const QObject *toolSet);
#ifdef QT_GUI_LIB
    void registerTool(QAction *action, const QString &name);
    void unregisterTool(const QAction *action);
#endif

    void setRoots(const QList<QMcpRoot> &roots);

    void createMessage(const QMcpCreateMessageRequestParams &params);

signals:
    void initializedChanged(bool initialized);
    void resourceUpdated(const QMcpResource &resource);
    void resourceListChanged();
    void promptListChanged();
    void toolListChanged();
    void rootsChanged(const QList<QMcpRoot> &roots);
    void createMessageFinished(const QMcpCreateMessageResult &result);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPSERVERSESSION_H
