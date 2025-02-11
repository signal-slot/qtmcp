// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserversse.h"
#include "httpserver.h"
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QLoggingCategory>
#include <QtNetwork/QTcpServer>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcQMcpServerSsePlugin, "qt.mcpserver.plugins.backend.sse")

class QMcpServerSse::Private
{
public:
    Private(QMcpServerSse *parent);

private:
    QMcpServerSse *q;
public:
    QTcpServer tcpServer;
    HttpServer httpServer;
    QSet<QUuid> sessions;
};

QMcpServerSse::Private::Private(QMcpServerSse *parent)
    : q(parent)
{

}

QMcpServerSse::QMcpServerSse(QObject *parent)
    : QMcpServerBackendInterface(parent)
    , d(new Private(this))
{}

QMcpServerSse::~QMcpServerSse() = default;

void QMcpServerSse::start(const QString &server)
{
    Q_UNUSED(server);
    QHostAddress address = QHostAddress::Any;
    quint16 port = 0;
    const int colon = server.indexOf(':');
    if (colon < 0) {
        address = QHostAddress(server);
    } else {
        address = QHostAddress(server.left(colon));
        port = server.mid(colon + 1).toInt();
    }
    if (!d->tcpServer.listen(address, port) || !d->httpServer.bind(&d->tcpServer)) {
        qWarning() << "server start failed." << server;
        return;
    }
    qDebug() << "Listening on port" << d->tcpServer.serverPort();
}

void QMcpServerSse::send(const QJsonObject &object)
{
    const auto data = QJsonDocument(object).toJson(QJsonDocument::Compact);
    qDebug() << data;
}

void QMcpServerSse::notify(const QJsonObject &object)
{
    send(object);
}

QT_END_NAMESPACE

#include "qmcpserversse.moc"
