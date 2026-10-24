// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QCoreApplication>
#include <QtCore/QDeadlineTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>
#include <QtTest/QTest>

using namespace Qt::Literals::StringLiterals;

namespace {

constexpr int Timeout = 5000;

QByteArray request(int id, const QString &method, const QJsonObject &params = {})
{
    const QJsonObject object {
        { "jsonrpc"_L1, "2.0"_L1 },
        { "id"_L1, id },
        { "method"_L1, method },
        { "params"_L1, params },
    };
    return QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
}

// Waits for the next line the server writes and parses it.
QJsonObject readResponse(QProcess *process)
{
    QDeadlineTimer deadline(Timeout);
    while (!process->canReadLine()) {
        if (deadline.hasExpired() || !process->waitForReadyRead(deadline.remainingTime()))
            return {};
    }
    return QJsonDocument::fromJson(process->readLine()).object();
}

} // namespace

// Runs a stdio server as a child process and talks to it over pipes, like an
// MCP client does.
class tst_Stdio : public QObject
{
    Q_OBJECT

private slots:
    void requestsOverPipes();
    void startedIsEmitted();

private:
    QString serverPath() const
    {
        return QCoreApplication::applicationDirPath() + "/stdioserver"_L1;
    }
};

void tst_Stdio::requestsOverPipes()
{
    QProcess server;
    server.setProcessChannelMode(QProcess::ForwardedErrorChannel);
    server.start(serverPath());
    QVERIFY2(server.waitForStarted(Timeout), qPrintable(server.errorString()));

    // Sent right after spawning, like the initialize request of a client.
    server.write(request(1, "initialize"_L1, {
        { "protocolVersion"_L1, "2025-11-25"_L1 },
        { "capabilities"_L1, QJsonObject() },
        { "clientInfo"_L1, QJsonObject { { "name"_L1, "tst_stdio"_L1 }, { "version"_L1, "1.0"_L1 } } },
    }));
    auto response = readResponse(&server);
    QCOMPARE(response.value("id"_L1).toInt(), 1);
    QVERIFY(response.contains("result"_L1));

    // A request after the server sat idle has to be picked up as well.
    QTest::qWait(200);
    server.write(request(2, "ping"_L1));
    response = readResponse(&server);
    QCOMPARE(response.value("id"_L1).toInt(), 2);
    QVERIFY(response.contains("result"_L1));

    // Closing STDIN is EOF, which ends the server.
    server.closeWriteChannel();
    QVERIFY(server.waitForFinished(Timeout));
    QCOMPARE(server.exitStatus(), QProcess::NormalExit);
    QCOMPARE(server.exitCode(), 0);
}

void tst_Stdio::startedIsEmitted()
{
    QProcess server;
    server.setProcessChannelMode(QProcess::ForwardedErrorChannel);
    server.start(serverPath(), { "--exit-on-started"_L1 });
    QVERIFY2(server.waitForStarted(Timeout), qPrintable(server.errorString()));

    // STDIN stays open, so only started() ends the server.
    const bool finished = server.waitForFinished(Timeout);
    if (!finished)
        server.kill();
    QVERIFY(finished);
    QCOMPARE(server.exitStatus(), QProcess::NormalExit);
    QCOMPARE(server.exitCode(), 0);
}

QTEST_MAIN(tst_Stdio)
#include "tst_stdio.moc"
