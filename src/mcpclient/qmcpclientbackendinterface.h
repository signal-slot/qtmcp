// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTBACKENDINTERFACE_H
#define QMCPCLIENTBACKENDINTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtMcpClient/qmcpclientglobal.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMcpClientBackendInterface
    \inmodule QtMcpClient
    \brief The QMcpClientBackendInterface class provides an interface for implementing MCP client backends.

    This class defines the interface that must be implemented by backend plugins to provide
    different transport mechanisms for MCP clients. For example, a backend might implement
    communication over stdio, TCP/IP, or other protocols.

    Backend implementations must handle:
    \list
    \li Starting and managing the connection to an MCP server
    \li Sending requests and notifications
    \li Receiving and dispatching responses and notifications
    \endlist
*/
class Q_MCPCLIENT_EXPORT QMcpClientBackendInterface : public QObject
{
    Q_OBJECT
public:
    /*!
        Constructs a client backend interface with the given parent.
        \param parent The parent object
    */
    explicit QMcpClientBackendInterface(QObject *parent = nullptr);

    /*!
        Sends a request to the server and optionally handles the response with a callback.
        
        \param request The request as a JSON object
        \param callback Optional callback to handle the response
    */
    void request(const QJsonObject &request, std::function<void(const QJsonObject &)> callback = nullptr);

public slots:
    /*!
        Starts the backend with the given server arguments.
        Must be implemented by backend classes.
        
        \param server Arguments for connecting to the server
    */
    virtual void start(const QString &server) = 0;

    /*!
        Sends a raw JSON object to the server.
        Must be implemented by backend classes.
        
        \param object The JSON object to send
    */
    virtual void send(const QJsonObject &object) = 0;

    /*!
        Sends a notification to the server.
        Must be implemented by backend classes.
        
        \param object The notification as a JSON object
    */
    virtual void notify(const QJsonObject &object) = 0;

signals:
    /*!
        Emitted when the backend has successfully started.
    */
    void started();

    /*!
        Emitted when an error occurs in the backend.
        \param error Description of the error
    */
    void errorOccurred(const QString &error);

    /*!
        Emitted when the backend connection is closed.
    */
    void finished();

    /*!
        Emitted when a raw JSON message is received from the server.
        \param object The received JSON message
    */
    void received(const QJsonObject &object);

    /*!
        Emitted when a result is received from the server.
        \param result The result as a JSON object
    */
    void result(const QJsonObject &result);

private:
    QHash<QJsonValue, std::function<void(const QJsonObject &)>> callbacks;
};

QT_END_NAMESPACE

#endif // QMCPCLIENTBACKENDINTERFACE_H
