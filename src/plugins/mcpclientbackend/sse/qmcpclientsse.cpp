// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpclientsse.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QLoggingCategory>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcQMcpClientSsePlugin, "qt.mcpclient.plugins.backend.sse")

class QMcpClientSse::Private
{
public:
    Private(QMcpClientSse *parent);

    void start(const QUrl &url);

private:
    QMcpClientSse *q;
public:
    QUrl sse;
    QUrl message;
    QNetworkAccessManager networkAccessManager;
private:
    QScopedPointer<QNetworkReply> eventStream;
};

QMcpClientSse::Private::Private(QMcpClientSse *parent)
    : q(parent)
{}

void QMcpClientSse::Private::start(const QUrl &url)
{
    sse = url;
    sse.setPath("/sse");
    QNetworkRequest request(sse);
    request.setRawHeader("Accept", "text/event-stream");
    request.setRawHeader("Cache-Control", "no-cache");

    eventStream.reset(networkAccessManager.get(request));

    connect(eventStream.data(), &QNetworkReply::readyRead, q, [this]() {
        static QByteArray data;
        const auto received = eventStream->readAll();
        qCDebug(lcQMcpClientSsePlugin) << received;
        data.append(received);
        QByteArray separator;
        while (true) {
            if (separator.isEmpty()) {
                if (data.contains("\r\n\r\n"))
                    separator = "\r\n";
                else if (data.contains("\n\n"))
                    separator = "\n";
                else
                    break;
            }
            int separator2 = data.indexOf(QByteArray(separator + separator));
            if (separator2 < 0)
                break;

            const auto chunk = data.left(separator2);
            data.remove(0, separator2 + separator.length() * 2);

            QByteArrayList lines;
            int from = 0;
            int separator1 = chunk.indexOf(separator, from);
            while (separator1 > 0) {
                lines.append(chunk.mid(from, separator1 - from));
                from = separator1 + separator.length();
                separator1 = chunk.indexOf(separator, from);
            }
            lines.append(chunk.mid(from));

            const auto firstLine = lines.at(0);
            int colon = firstLine.indexOf(':');
            const auto type = firstLine.left(colon);
            const auto key = firstLine.mid(colon + 2);
            if (type.isEmpty()) {
                if (key.startsWith("ping")) {
                    continue;
                }
                qCWarning(lcQMcpClientSsePlugin) << "unknown type" << key;
            } else if (type == "event") {
                auto data = lines.at(1);
                if (!data.startsWith("data: ")) {
                    qCWarning(lcQMcpClientSsePlugin) << data;
                    continue;
                }
                data = data.mid(6);
                if (key == "endpoint") {
                    message = sse;
                    qCDebug(lcQMcpClientSsePlugin) << message << sse;
                    int question = data.indexOf('?');
                    if (question < 0) {
                        message.setPath(data);
                        qCDebug(lcQMcpClientSsePlugin) << message << sse;
                    } else {
                        message.setPath(data.left(question));
                        qCDebug(lcQMcpClientSsePlugin) << message << sse;
                        message.setQuery(data.mid(question + 1));
                        qCDebug(lcQMcpClientSsePlugin) << message << sse;
                    }
                    emit q->started();
                } else if (key == "message") {
                    QJsonParseError error;
                    const auto json = QJsonDocument::fromJson(data, &error);
                    if (error.error) {
                        qCWarning(lcQMcpClientSsePlugin) << error.errorString();
                    } else {
                        emit q->received(json.object());
                    }
                } else {
                    qCWarning(lcQMcpClientSsePlugin) << "unknown key" << key;
                }
            }
        }
    });
    connect(eventStream.data(), &QNetworkReply::finished, q, &QMcpClientSse::finished);
    connect(eventStream.data(), &QNetworkReply::sslErrors, q, [this](const QList<QSslError> &errors) {
        for (const QSslError &error : errors)
            qCDebug(lcQMcpClientSsePlugin) << error.errorString();
        eventStream->ignoreSslErrors();
    });
    connect(eventStream.data(), &QNetworkReply::errorOccurred, q, [this](QNetworkReply::NetworkError error) {
        qCWarning(lcQMcpClientSsePlugin) << error;
        emit q->errorOccurred(eventStream->errorString());
    });
}

QMcpClientSse::QMcpClientSse(QObject *parent)
    : QMcpClientBackendInterface(parent)
    , d(new Private(this))
{}

QMcpClientSse::~QMcpClientSse() = default;

void QMcpClientSse::start(const QString &server)
{
    d->start(QUrl(server));
}

void QMcpClientSse::send(const QJsonObject &object)
{
    if (d->message.isEmpty()) {
        qCWarning(lcQMcpClientSsePlugin) << d->message;
        return;
    }
    QNetworkRequest request(d->message);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonDocument doc(object);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    qCDebug(lcQMcpClientSsePlugin) << data;

    auto *reply = d->networkAccessManager.post(request, data);
    connect(reply, &QNetworkReply::sslErrors, this, [reply](const QList<QSslError> &errors) {
        for (const QSslError &error : errors)
            qCDebug(lcQMcpClientSsePlugin) << error.errorString();
        reply->ignoreSslErrors();
    });
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
    connect(reply, &QNetworkReply::errorOccurred, this, [reply](QNetworkReply::NetworkError error) {
        qCDebug(lcQMcpClientSsePlugin) << error << reply->errorString();
    });
}

void QMcpClientSse::notify(const QJsonObject &object)
{
    send(object); // For SSE, notifications are sent the same way as regular messages
}

QT_END_NAMESPACE
