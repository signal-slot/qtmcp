// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERSTDIO_H
#define QMCPSERVERSTDIO_H

#include <QtMcpServer/qmcpserverbackendplugin.h>
#include <QtMcpServer/qmcpserverbackendinterface.h>
#include <QtCore/QJsonObject>

QT_BEGIN_NAMESPACE

class QMcpServerStdio : public QMcpServerBackendInterface
{
    Q_OBJECT
public:
    explicit QMcpServerStdio(QObject *parent = nullptr);
    ~QMcpServerStdio() override;

public slots:
    void start(const QString &server) override;
    void send(const QUuid &session, const QJsonObject &object) override;
    void notify(const QUuid &session, const QJsonObject &object) override;

private:
    class Private;
    QScopedPointer<Private> d;
};

class QMcpServerStdioPlugin : public QMcpServerBackendPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QMcpServerBackendPluginFactoryInterface_iid FILE "qmcpserverstdio.json")
public:
    QMcpServerBackendInterface *create(const QString &key, QObject *parent = nullptr) override
    {
        Q_ASSERT(key == "stdio"_L1);
        return new QMcpServerStdio(parent);
    }
};

QT_END_NAMESPACE

#endif // QMCPSERVERSTDIO_H
