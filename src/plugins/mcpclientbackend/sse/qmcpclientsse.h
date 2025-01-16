// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTSSE_H
#define QMCPCLIENTSSE_H

#include <QtMcpClient/qmcpclientbackendinterface.h>
#include <QtMcpClient/qmcpclientbackendplugin.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE

class QMcpClientSse : public QMcpClientBackendInterface
{
    Q_OBJECT
public:
    explicit QMcpClientSse(QObject *parent = nullptr);
    ~QMcpClientSse() override;

public slots:
    void start(const QString &server) override;
    void send(const QJsonObject &object) override;
    void notify(const QJsonObject &object) override;

private:
    class Private;
    QScopedPointer<Private> d;
};

class QMcpClientSsePlugin : public QMcpClientBackendPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QMcpClientBackendPluginFactoryInterface_iid FILE "qmcpclientsse.json")
public:
    QMcpClientBackendInterface *create(const QString &key, QObject *parent) override
    {
        static const QStringList keys { "sse"_L1 };
        Q_ASSERT(keys.contains(key));
        return new QMcpClientSse(parent);
    }
};

QT_END_NAMESPACE

#endif // QMCPCLIENTSSE_H
