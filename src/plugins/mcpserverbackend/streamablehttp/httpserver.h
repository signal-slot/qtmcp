// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <QtMcpServer/qmcpabstracthttpserver.h>
#include <QtNetwork/QNetworkRequest>

/*!
    \class HttpServer
    \internal
    \brief Implements the MCP Streamable HTTP transport on a single \c /mcp endpoint.

    The transport behaves differently depending on the revision a request
    declares through its \c MCP-Protocol-Version header:

    \list
    \li 2025-03-26 .. 2025-11-25: \c initialize mints a session that is returned
        in the \c Mcp-Session-Id response header and echoed back by the client
        on every subsequent request. \c GET opens a standalone SSE stream for
        that session, \c DELETE terminates it.
    \li 2026-07-28: sessions are gone from the wire. Ordinary requests run on a
        single shared, stateless session; \c subscriptions/listen gets a
        dedicated one because the core routes notifications per session.
        \c GET and \c DELETE are not allowed.
    \endlist
*/
class HttpServer : public QMcpAbstractHttpServer
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer() override;

    /*!
        Origins accepted in addition to the loopback ones. Validating \c Origin
        is what keeps a locally bound MCP server safe from DNS rebinding.
    */
    QStringList allowedOrigins() const;
    void setAllowedOrigins(const QStringList &origins);

    /*!
        Creates the shared session that carries every stateless (2026-07-28)
        request. Call it once newSession() is connected.
    */
    void startStatelessSession();

    Q_INVOKABLE QByteArray postMcp(const QNetworkRequest &request, const QByteArray &body);
    Q_INVOKABLE QByteArray getMcp(const QNetworkRequest &request);
    Q_INVOKABLE QByteArray deleteMcp(const QNetworkRequest &request);

public slots:
    void send(const QUuid &session, const QJsonObject &object);

signals:
    void newSession(const QUuid &session);
    void received(const QUuid &session, const QJsonObject &object);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // HTTPSERVER_H
