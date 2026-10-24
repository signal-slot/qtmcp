// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserverstdio.h"
#include <QtCore/QLoggingCategory>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtCore/QSocketNotifier>
#ifdef Q_OS_WIN
#include <QtCore/QTimer>
#include <QtCore/qt_windows.h>
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
    enum class ReadResult {
        Continue,
        // STDIN reached EOF or failed
        Finished,
        // A slot connected to received() deleted the backend
        Deleted,
    };
    ReadResult readData();
    void stopReading();
#ifdef Q_OS_WIN
    void pollPipe();
#endif

private:
    QMcpServerStdio *q;
    QSocketNotifier *notifier = nullptr;
#ifdef Q_OS_WIN
    // QSocketNotifier only works with sockets on Windows and never fires for
    // a pipe, which is what MCP clients connect to STDIN, so a pipe is polled
    // instead.
    QTimer *pipeTimer = nullptr;
#endif
    // Data read from STDIN that does not form a complete line yet
    QByteArray data;
    const QUuid uuid = QUuid::createUuid();
};

QMcpServerStdio::Private::Private(QMcpServerStdio *parent)
    : q(parent)
{
    QMetaObject::invokeMethod(q, "newSessionStarted", Qt::QueuedConnection, Q_ARG(QUuid, uuid));
#ifdef Q_OS_WIN
    // Set stdin to binary mode to avoid CRLF translation
    _setmode(_fileno(stdin), _O_BINARY);
    if (GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_PIPE) {
        pipeTimer = new QTimer(q);
        pipeTimer->setInterval(10);
        connect(pipeTimer, &QTimer::timeout, q, [this] { pollPipe(); });
        pipeTimer->start();
        return;
    }
#endif
    notifier = new QSocketNotifier(STDIN_FILENO, QSocketNotifier::Read, q);
    connect(notifier, &QSocketNotifier::activated, q, [this] {
        if (readData() == ReadResult::Finished)
            emit q->finished();
    });
}

void QMcpServerStdio::Private::stopReading()
{
    if (notifier)
        notifier->setEnabled(false);
#ifdef Q_OS_WIN
    if (pipeTimer)
        pipeTimer->stop();
#endif
}

#ifdef Q_OS_WIN
void QMcpServerStdio::Private::pollPipe()
{
    // PeekNamedPipe tells how much can be read without blocking, so the event
    // loop keeps running while the client is idle.
    const HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
    while (true) {
        DWORD available = 0;
        if (!PeekNamedPipe(in, nullptr, 0, nullptr, &available, nullptr)) {
            // ERROR_BROKEN_PIPE means the client closed its end, i.e. EOF
            const auto error = GetLastError();
            if (error != ERROR_BROKEN_PIPE)
                qCWarning(lcQMcpServerStdioPlugin) << "PeekNamedPipe failed:" << error;
            stopReading();
            emit q->finished();
            return;
        }
        if (available == 0)
            return;
        switch (readData()) {
        case ReadResult::Continue:
            break;
        case ReadResult::Finished:
            emit q->finished();
            return;
        case ReadResult::Deleted:
            return;
        }
    }
}
#endif

QMcpServerStdio::Private::ReadResult QMcpServerStdio::Private::readData()
{
    // Read available data from STDIN (non-blocking, as data is ready)
    char buffer[1024];
    const auto bytesRead = ::read(STDIN_FILENO, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        std::perror("Error reading STDIN");
        stopReading();
        return ReadResult::Finished;
    }
    if (bytesRead == 0) {
        // EOF reached (no more data)
        stopReading();
        return ReadResult::Finished;
    }

    // Append to buffer (may accumulate partial data from previous reads)
    data.append(buffer, static_cast<int>(bytesRead));

    // A slot connected to received() may delete q, and this along with it
    const QPointer<QMcpServerStdio> guard(q);

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

        emit q->received(uuid, jsonDoc.object());
        if (!guard)
            return ReadResult::Deleted;
    }
    return ReadResult::Continue;
}

QMcpServerStdio::QMcpServerStdio(QObject *parent)
    : QMcpServerBackendInterface(parent)
    , d(new Private(this))
{}

QMcpServerStdio::~QMcpServerStdio() = default;

void QMcpServerStdio::start(const QString &server)
{
    Q_UNUSED(server);
    // STDIN is read from construction on, so there is nothing left to set up.
    emit started();
}

void QMcpServerStdio::send(const QUuid &session, const QJsonObject &object)
{
    Q_UNUSED(session)
    const auto data = QJsonDocument(object).toJson(QJsonDocument::Compact);
    qCDebug(lcQMcpServerStdioPlugin) << data;
    std::cout << data.toStdString() << std::endl;
}

void QMcpServerStdio::notify(const QUuid &session, const QJsonObject &object)
{
    send(session, object);
}

QT_END_NAMESPACE
