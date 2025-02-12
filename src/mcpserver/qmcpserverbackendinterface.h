// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERBACKENDINTERFACE_H
#define QMCPSERVERBACKENDINTERFACE_H

#include <QtMcpServer/qmcpserverglobal.h>
#include <QtMcpCommon/QMcpRequest>
#include <QtMcpCommon/QMcpResult>

QT_BEGIN_NAMESPACE

/*!
    \class QMcpServerBackendInterface
    \inmodule QtMcpServer
    \brief The QMcpServerBackendInterface class provides an interface for implementing MCP server backends.

    This class defines the interface that must be implemented by backend plugins to provide
    different transport mechanisms for MCP servers. For example, a backend might implement
    communication over stdio, TCP/IP, or other protocols.

    Backend implementations must handle:
    \list
    \li Starting and managing client connections
    \li Managing multiple client sessions
    \li Sending requests and notifications to specific clients
    \li Receiving and dispatching client messages
    \endlist
*/
class Q_MCPSERVER_EXPORT QMcpServerBackendInterface : public QObject
{
    Q_OBJECT
public:
    /*!
        Constructs a server backend interface with the given parent.
        \param parent The parent object
    */
    explicit QMcpServerBackendInterface(QObject *parent = nullptr);

    /*!
        Sends a request to a specific client session and optionally handles the response with a callback.
        
        \param session UUID of the client session
        \param request The request as a JSON object
        \param callback Optional callback to handle the response
    */
    void request(const QUuid &session, const QJsonObject &request, std::function<void(const QJsonObject &)> callback = nullptr);

public slots:
    /*!
        Starts the backend with the given server arguments.
        Must be implemented by backend classes.
        
        \param server Arguments for starting the server
    */
    virtual void start(const QString &server) = 0;

    /*!
        Sends a raw JSON object to a specific client session.
        Must be implemented by backend classes.
        
        \param session UUID of the client session
        \param object The JSON object to send
    */
    virtual void send(const QUuid &session, const QJsonObject &object) = 0;

    /*!
        Sends a notification to a specific client session.
        Must be implemented by backend classes.
        
        \param session UUID of the client session
        \param object The notification as a JSON object
    */
    virtual void notify(const QUuid &session, const QJsonObject &object) = 0;

signals:
    /*!
        Emitted when a new client session is established.
        \param session UUID of the new client session
    */
    void newSessionStarted(const QUuid &session);

    /*!
        Emitted when the backend has successfully started.
    */
    void started();

    /*!
        Emitted when the backend is shutting down.
    */
    void finished();

    /*!
        Emitted when a raw JSON message is received from a client.
        \param session UUID of the client session
        \param object The received JSON message
    */
    void received(const QUuid &session, const QJsonObject &object);

    /*!
        Emitted when a result is ready to be sent to a client.
        \param session UUID of the client session
        \param result The result as a JSON object
    */
    void result(const QUuid &session, const QJsonObject &result);

private:
    QHash<QUuid, QHash<QJsonValue, std::function<void(const QJsonObject &)>>> callbacks;
};

QT_END_NAMESPACE

#endif // QMCPSERVERBACKENDINTERFACE_H
