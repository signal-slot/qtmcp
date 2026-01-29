// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpclientstdio.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QProcess>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <QtCore/QMetaEnum>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcQMcpClientStdioPlugin, "qt.mcpclient.plugins.backend.stdio")

class QMcpClientStdio::Private
{
public:
    Private(QMcpClientStdio *parent);

private:
    QMcpClientStdio *q;

public:
    QProcess server;
};

QMcpClientStdio::Private::Private(QMcpClientStdio *parent)
    : q(parent)
{
    connect(&server, &QProcess::stateChanged, q, [](QProcess::ProcessState state) {
        qDebug() << state;
    });
    connect(&server, &QProcess::errorOccurred, q, [this](QProcess::ProcessError error) {
        qWarning() << error << server.errorString();
        emit q->errorOccurred(server.errorString());
    });
    connect(&server, &QProcess::started, q, [this]() {
        emit q->started();
    });
    connect(&server, &QProcess::finished, q, [this]() {
        emit q->finished();
    });
    connect(&server, &QProcess::readyReadStandardOutput, q, [this]() {
        static QByteArray data;
        data.append(server.readAllStandardOutput());
        while (true) {
            int lf = data.indexOf('\n');
            if (lf < 0)
                break;
            const auto line = data.left(lf);
            data.remove(0, lf + 1);
            QJsonParseError error;
            const auto json = QJsonDocument::fromJson(line, &error);
            if (error.error) {
                qWarning() << error.errorString();
            } else {
                qDebug() << json;
                emit q->received(json.object());
            }
        }
    });
    connect(&server, &QProcess::readyReadStandardError, q, [this]() {
        qWarning() << server.readAllStandardError();
    });
}

QMcpClientStdio::QMcpClientStdio(QObject *parent)
    : QMcpClientBackendInterface(parent)
    , d(new Private(this))
{}

QMcpClientStdio::~QMcpClientStdio() = default;

void QMcpClientStdio::start(const QString &server)
{
    QStringList arguments = server.split(' ');
    QString program = arguments.takeFirst();
    d->server.start(program, arguments);
}

void QMcpClientStdio::send(const QJsonObject &object)
{
    qDebug() << d->server.state();
    const auto data = QJsonDocument(object).toJson(QJsonDocument::Compact);
    qDebug().noquote() << data;
    d->server.write(data + "\n");
}

void QMcpClientStdio::notify(const QJsonObject &object)
{
    send(object);
}

QT_END_NAMESPACE
