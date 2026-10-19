// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserverstdio.h"
#include <QtCore/QDebug>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QLoggingCategory>
#include <QtCore/QSocketNotifier>
#include <qtimer.h>
#ifdef Q_OS_WIN
#include <fcntl.h>
#include <io.h>
#define STDIN_FILENO _fileno(stdin)
#define read _read
#else
#include <unistd.h> // for STDIN_FILENO and ::read on POSIX
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
    void pollStdin(); // Timer fallback
private:
    QMcpServerStdio *q;
    QSocketNotifier *notifier;
    QTimer *pollTimer;
    QByteArray dataBuf; // Move static local buffer into class member dataBuf
    const QUuid uuid = QUuid::createUuid();
};

QMcpServerStdio::Private::Private(QMcpServerStdio *parent)
    : q(parent)
    , notifier(new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, q))
    , pollTimer(new QTimer(q))
    , dataBuf()
{
#ifdef Q_OS_WIN
    // Set stdin to binary mode to avoid CRLF translation
    _setmode(_fileno(stdin), _O_BINARY);
#endif
    QMetaObject::invokeMethod(q, "newSessionStarted", Qt::QueuedConnection, Q_ARG(QUuid, uuid));
    connect(notifier, &QSocketNotifier::activated, q, [this](QSocketDescriptor socket, QSocketNotifier::Type activationEvent) { readData(socket, activationEvent); });

    // Fallback polling, every 50ms
    pollTimer->setInterval(50);
    connect(pollTimer, &QTimer::timeout, q, [this]() { pollStdin(); });
    pollTimer->start();
}

void QMcpServerStdio::Private::readData(QSocketDescriptor socket, QSocketNotifier::Type type)
{
    Q_UNUSED(socket);
    Q_UNUSED(type);
    // Read available data from STDIN (non-blocking, as data is ready)
    char buffer[1024];
    const auto bytesRead = ::read(STDIN_FILENO, buffer, sizeof(buffer));
    if (bytesRead < 0)
    {
        std::perror("Error reading STDIN");
        notifier->setEnabled(false);
        emit q->finished();
        return;
    }
    if (bytesRead == 0)
    {
        // EOF reached (no more data)
        notifier->setEnabled(false);
        emit q->finished();
        return;
    }

    // Append to buffer (may accumulate partial data from previous reads)
    // No longer use "static", instead use the member "dataBuf"
    dataBuf.append(buffer, static_cast<int>(bytesRead));

    // Process complete lines in the buffer (assuming JSON messages are newline-terminated)
    while (true)
    {
        int newlineIndex = dataBuf.indexOf('\n');
        if (newlineIndex < 0)
            break;

        QByteArray lineData = dataBuf.left(newlineIndex);
        dataBuf.remove(0, newlineIndex + 1); // remove this line from buffer

        // Trim to avoid empty lines
        QByteArray jsonData = lineData.trimmed();
        if (jsonData.isEmpty())
        {
            continue;
        }

        // Parse JSON data
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
        if (parseError.error != QJsonParseError::NoError)
        {
            qWarning() << "JSON parse error: " << parseError.errorString().toStdString();
            continue;
        }

        if (!jsonDoc.isObject())
        {
            qWarning() << "JSON is not an object" << jsonDoc;
            continue;
        }

        emit q->received(uuid, jsonDoc.object());
    }
}

void QMcpServerStdio::Private::pollStdin()
{
    readData({}, {});
}

QMcpServerStdio::QMcpServerStdio(QObject *parent)
    : QMcpServerBackendInterface(parent)
    , d(new Private(this))
{
}

QMcpServerStdio::~QMcpServerStdio() = default;

void QMcpServerStdio::start(const QString &server)
{
    Q_UNUSED(server);
}

void QMcpServerStdio::send(const QUuid &session, const QJsonObject &object)
{
    Q_UNUSED(session)
    const auto data = QJsonDocument(object).toJson(QJsonDocument::Compact);
    qCDebug(lcQMcpServerStdioPlugin) << data;
    // In Windows pipes, using the explicit `std::cout.flush()` is more controllable.
    std::cout << data.toStdString() << '\n';
    std::cout.flush();
}

void QMcpServerStdio::notify(const QUuid &session, const QJsonObject &object)
{
    send(session, object);
}

QT_END_NAMESPACE
