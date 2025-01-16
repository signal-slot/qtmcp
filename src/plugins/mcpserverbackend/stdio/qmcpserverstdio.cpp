// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserverstdio.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>
#include <QtCore/QSocketNotifier>
#ifdef Q_OS_WIN
#include <io.h>
#include <fcntl.h>
#define STDIN_FILENO _fileno(stdin)
#define read _read
#else
#include <unistd.h>   // for STDIN_FILENO and ::read on POSIX
#endif
#include <iostream>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcQMcpServerStdioPlugin, "qt.mcpserver.plugins.backend.stdio")

class QMcpServerStdio::Private
{
public:
    Private(QMcpServerStdio *parent);

private:
    void readData(QSocketDescriptor socket, QSocketNotifier::Type activationEvent);

private:
    QMcpServerStdio *q;
    QSocketNotifier *notifier;
};

QMcpServerStdio::Private::Private(QMcpServerStdio *parent)
    : q(parent)
    , notifier(new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, q))
{
#ifdef Q_OS_WIN
    // Set stdin to binary mode to avoid CRLF translation
    _setmode(_fileno(stdin), _O_BINARY);
#endif
    connect(notifier, &QSocketNotifier::activated, q, [this](QSocketDescriptor socket, QSocketNotifier::Type activationEvent) {
        readData(socket, activationEvent);
    });
}

void QMcpServerStdio::Private::readData(QSocketDescriptor socket, QSocketNotifier::Type type) {
    Q_UNUSED(socket);
    Q_UNUSED(type);
    // Read available data from STDIN (non-blocking, as data is ready)
    char buffer[1024];
    const auto bytesRead = ::read(STDIN_FILENO, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        std::perror("Error reading STDIN");
        return;
    }
    if (bytesRead == 0) {
        // EOF reached (no more data)
        emit q->finished();
        return;
    }

    // Append to buffer (may accumulate partial data from previous reads)
    static QByteArray data;
    data.append(buffer, static_cast<int>(bytesRead));

    // Process complete lines in the buffer (assuming JSON messages are newline-terminated)
    while (true) {
        int newlineIndex = data.indexOf('\n');
        if (newlineIndex < 0)
            break;

        QByteArray lineData = data.left(newlineIndex);
        data.remove(0, newlineIndex + 1);  // remove this line from buffer

        // Trim to avoid empty lines
        QByteArray jsonData = lineData.trimmed();
        if (jsonData.isEmpty()) {
            continue;
        }

        // Parse JSON data
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error: "
                       << parseError.errorString().toStdString();
            continue;
        }

        if (!jsonDoc.isObject()) {
            qWarning() << "JSON is not an object" << jsonDoc;
            continue;
        }

        emit q->received(jsonDoc.object());
    }
}

QMcpServerStdio::QMcpServerStdio(QObject *parent)
    : QMcpServerBackendInterface(parent)
    , d(new Private(this))
{}

QMcpServerStdio::~QMcpServerStdio() = default;

void QMcpServerStdio::start(const QString &server)
{
    Q_UNUSED(server);
}

void QMcpServerStdio::send(const QJsonObject &object)
{
    const auto data = QJsonDocument(object).toJson(QJsonDocument::Compact);
    qDebug() << data;
    std::cout << data.toStdString() << std::endl;
}

void QMcpServerStdio::notify(const QJsonObject &object)
{
    send(object);
}

QT_END_NAMESPACE

#include "qmcpserverstdio.moc"
