// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERSESSION_H
#define QMCPSERVERSESSION_H

#include <QtCore/QFuture>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QUuid>
#include <QtMcpCommon/QMcpCallToolResult>
#include <QtMcpCommon/QMcpCallToolResultContent>
#include <QtMcpCommon/QMcpCreateMessageRequestParams>
#include <QtMcpCommon/QMcpCreateMessageResult>
#include <QtMcpCommon/QMcpElicitRequestParams>
#include <QtMcpCommon/QMcpElicitResult>
#include <QtMcpCommon/QMcpSubscriptionFilter>
#include <QtMcpCommon/QMcpPrompt>
#include <QtMcpCommon/QMcpPromptMessage>
#include <QtMcpCommon/QMcpReadResourceResultContents>
#include <QtMcpCommon/QMcpResource>
#include <QtMcpCommon/QMcpResourceTemplate>
#include <QtMcpCommon/QMcpRoot>
#include <QtMcpCommon/QMcpTool>
#include <QtMcpCommon/qtmcpnamespace.h>
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
        Returns the protocol version negotiated with the client.
    */
    QtMcp::ProtocolVersion protocolVersion() const;
    
    /*!
        Sets the protocol version negotiated with the client.
    */
    void setProtocolVersion(QtMcp::ProtocolVersion protocolVersion);
    
    /*!
        Sets the protocol version negotiated with the client using a string.
        \internal
        This is provided for backward compatibility.
    */
    void setProtocolVersion(const QString &protocolVersionStr);

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
        Executes a tool asynchronously with progress support.
        \param name Name of the tool to execute
        \param params Parameters for the tool
        \param progressToken Token for progress notifications (from request _meta)
        \return Future containing the tool execution result including isError flag
     */
    QFuture<QMcpCallToolResult> callToolAsync(const QString &name, const QJsonObject &params, const QVariant &progressToken = {});

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

    // Requires a client that negotiated MCP 2025-06-18 or later with the
    // elicitation capability.
    void elicit(const QMcpElicitRequestParams &params);

    // subscriptions/listen opt-ins (2026-07-28). Sessions on that revision
    // only receive the change notifications they subscribed to.
    bool hasListenSubscriptions() const;
    QMcpSubscriptionFilter listenSubscriptions() const;
    void setListenSubscriptions(const QMcpSubscriptionFilter &filter);
    QString listenSubscriptionId() const;

    // Multi round-trip requests (2026-07-28). During request handling a tool
    // or handler reads the input the client supplied on a retry with
    // inputResponses(), and calls requireInput() when it cannot finish
    // without more; the server then answers with an input_required interim
    // result instead of the handler's result. requestState round-trips
    // through the client, so the server stays stateless.
    QJsonObject inputResponses() const;
    QJsonValue clientRequestState() const;
    void requireInput(const QJsonObject &inputRequests, const QJsonValue &requestState = QJsonValue());
    static QJsonObject elicitationInputRequest(const QMcpElicitRequestParams &params);

    // Internal plumbing for QMcpServer.
    void provideInputResponses(const QJsonObject &responses, const QJsonValue &requestState);
    bool takeRequiredInput(QJsonObject *inputRequests, QJsonValue *requestState);

    // The client capabilities carried in the request _meta of a stateless
    // (2026-07-28) session, e.g. declared extensions. Updated per request.
    QJsonObject clientCapabilitiesJson() const;
    void setClientCapabilitiesJson(const QJsonObject &capabilities);

    // Replaces the pending request's result with a pre-serialized object,
    // e.g. a CreateTaskResult from the tasks extension. Internal.
    void overrideResult(const QJsonObject &result);
    QJsonObject takeResultOverride();

signals:
    void initializedChanged(bool initialized);
    void resourceUpdated(const QMcpResource &resource);
    void resourceListChanged();
    void promptListChanged();
    void toolListChanged();
    void rootsChanged(const QList<QMcpRoot> &roots);
    void createMessageFinished(const QMcpCreateMessageResult &result);
    void elicitFinished(const QMcpElicitResult &result);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPSERVERSESSION_H
