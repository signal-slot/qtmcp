// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTSTDIO_H
#define QMCPCLIENTSTDIO_H

#include <QtMcpClient/qmcpclientbackendplugin.h>
#include <QtMcpClient/qmcpclientbackendinterface.h>
#include <QtCore/QJsonObject>

QT_BEGIN_NAMESPACE

class QMcpClientStdio : public QMcpClientBackendInterface
{
    Q_OBJECT
public:
    explicit QMcpClientStdio(QObject *parent = nullptr);
    ~QMcpClientStdio() override;

public slots:
    void start(const QString &server) override;
    void send(const QJsonObject &object) override;
    void notify(const QJsonObject &object) override;

private:
    class Private;
    QScopedPointer<Private> d;
};

class QMcpClientStdioPlugin : public QMcpClientBackendPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QMcpClientBackendPluginFactoryInterface_iid FILE "qmcpclientstdio.json")
public:
    QMcpClientBackendInterface *create(const QString &key, QObject *parent = nullptr) override
    {
        Q_ASSERT(key == "stdio"_L1);
        return new QMcpClientStdio(parent);
    }
};

QT_END_NAMESPACE

#endif // QMCPCLIENTSTDIO_H
