// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERBACKENDINTERFACE_H
#define QMCPSERVERBACKENDINTERFACE_H

#include <QtMcpServer/qmcpserverglobal.h>
#include <QtMcpCommon/QMcpRequest>
#include <QtMcpCommon/QMcpResult>

QT_BEGIN_NAMESPACE

class Q_MCPSERVER_EXPORT QMcpServerBackendInterface : public QObject
{
    Q_OBJECT
public:
    explicit QMcpServerBackendInterface(QObject *parent = nullptr);

    void request(const QJsonObject &request, std::function<void(const QJsonObject &)> callback = nullptr);

public slots:
    virtual void start(const QString &server) = 0;
    virtual void send(const QJsonObject &object) = 0;
    virtual void notify(const QJsonObject &object) = 0;

signals:
    void started();
    void finished();
    void received(const QJsonObject &object);
    void result(const QJsonObject &result);

private:
    QHash<QJsonValue, std::function<void(const QJsonObject &)>> callbacks;
};

QT_END_NAMESPACE

#endif // QMCPSERVERBACKENDINTERFACE_H
