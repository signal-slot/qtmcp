// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERSSE_H
#define QMCPSERVERSSE_H

#include <QtMcpServer/qmcpserverbackendplugin.h>
#include <QtMcpServer/qmcpserverbackendinterface.h>
#include <QtCore/QJsonObject>

QT_BEGIN_NAMESPACE

class QMcpServerSse : public QMcpServerBackendInterface
{
    Q_OBJECT
public:
    explicit QMcpServerSse(QObject *parent = nullptr);
    ~QMcpServerSse() override;

public slots:
    void start(const QString &server) override;
    void send(const QUuid &session, const QJsonObject &object) override;
    void notify(const QUuid &session, const QJsonObject &object) override;

private:
    class Private;
    QScopedPointer<Private> d;
};

class QMcpServerSsePlugin : public QMcpServerBackendPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QMcpServerBackendPluginFactoryInterface_iid FILE "qmcpserversse.json")
public:
    QMcpServerBackendInterface *create(const QString &key, QObject *parent = nullptr) override
    {
        Q_ASSERT(key == "sse"_L1);
        return new QMcpServerSse(parent);
    }
};

QT_END_NAMESPACE

#endif // QMCPSERVERSSE_H
