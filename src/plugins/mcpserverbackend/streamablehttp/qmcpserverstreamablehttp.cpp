// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserverstreamablehttp.h"
#include "httpserver.h"

#include <QtCore/QLoggingCategory>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>

// Defined in httpserver.cpp, which lives outside the Qt namespace.
Q_DECLARE_LOGGING_CATEGORY(lcQMcpServerStreamableHttpPlugin)

QT_BEGIN_NAMESPACE

class QMcpServerStreamableHttp::Private
{
public:
    QTcpServer tcpServer;
    HttpServer httpServer;
};

QMcpServerStreamableHttp::QMcpServerStreamableHttp(QObject *parent)
    : QMcpServerBackendInterface(parent)
    , d(new Private)
{
    connect(&d->httpServer, &HttpServer::newSession,
            this, &QMcpServerStreamableHttp::newSessionStarted);
    connect(&d->httpServer, &HttpServer::received,
            this, &QMcpServerStreamableHttp::received);
}

QMcpServerStreamableHttp::~QMcpServerStreamableHttp() = default;

QStringList QMcpServerStreamableHttp::allowedOrigins() const
{
    return d->httpServer.allowedOrigins();
}

void QMcpServerStreamableHttp::setAllowedOrigins(const QStringList &allowedOrigins)
{
    if (d->httpServer.allowedOrigins() == allowedOrigins)
        return;
    d->httpServer.setAllowedOrigins(allowedOrigins);
    emit allowedOriginsChanged(allowedOrigins);
}

void QMcpServerStreamableHttp::start(const QString &server)
{
    QHostAddress address = QHostAddress::Any;
    quint16 port = 0;
    const auto colon = server.indexOf(':');
    if (colon < 0) {
        address = QHostAddress(server);
    } else {
        address = QHostAddress(server.left(colon));
        port = server.mid(colon + 1).toUShort();
    }
    if (!d->tcpServer.listen(address, port) || !d->httpServer.bind(&d->tcpServer)) {
        qWarning() << "server start failed." << server;
        return;
    }
    qCDebug(lcQMcpServerStreamableHttpPlugin) << "Listening on port" << d->tcpServer.serverPort();

    // 2026-07-28 has no initialize handshake, so the session its requests run
    // on cannot be created on demand by one of them; it exists up front.
    d->httpServer.startStatelessSession();

    emit started();
}

void QMcpServerStreamableHttp::send(const QUuid &session, const QJsonObject &object)
{
    qCDebug(lcQMcpServerStreamableHttpPlugin) << "Sending message:" << session;
    d->httpServer.send(session, object);
}

void QMcpServerStreamableHttp::notify(const QUuid &session, const QJsonObject &object)
{
    send(session, object);
}

QT_END_NAMESPACE
