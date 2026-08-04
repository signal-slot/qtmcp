// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERSTREAMABLEHTTP_H
#define QMCPSERVERSTREAMABLEHTTP_H

#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <QtMcpServer/qmcpserverbackendinterface.h>
#include <QtMcpServer/qmcpserverbackendplugin.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMcpServerStreamableHttp
    \internal
    \brief Serves MCP over the Streamable HTTP transport.

    Every revision from 2025-03-26 up to 2026-07-28 is served from the same
    \c /mcp endpoint; a request selects the one it speaks with its
    \c MCP-Protocol-Version header.
*/
class QMcpServerStreamableHttp : public QMcpServerBackendInterface
{
    Q_OBJECT
    /*!
        \property QMcpServerStreamableHttp::allowedOrigins
        Origins accepted besides \c http://localhost* and \c http://127.0.0.1*.
        Requests from any other origin are rejected with 403, which is what
        protects a locally bound server against DNS rebinding.
    */
    Q_PROPERTY(QStringList allowedOrigins READ allowedOrigins WRITE setAllowedOrigins
               NOTIFY allowedOriginsChanged)
public:
    explicit QMcpServerStreamableHttp(QObject *parent = nullptr);
    ~QMcpServerStreamableHttp() override;

    QStringList allowedOrigins() const;

public slots:
    void start(const QString &server) override;
    void send(const QUuid &session, const QJsonObject &object) override;
    void notify(const QUuid &session, const QJsonObject &object) override;
    void setAllowedOrigins(const QStringList &allowedOrigins);

signals:
    void allowedOriginsChanged(const QStringList &allowedOrigins);

private:
    class Private;
    QScopedPointer<Private> d;
};

class QMcpServerStreamableHttpPlugin : public QMcpServerBackendPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QMcpServerBackendPluginFactoryInterface_iid
                      FILE "qmcpserverstreamablehttp.json")
public:
    QMcpServerBackendInterface *create(const QString &key, QObject *parent = nullptr) override
    {
        Q_ASSERT(key == "streamablehttp"_L1);
        return new QMcpServerStreamableHttp(parent);
    }
};

QT_END_NAMESPACE

#endif // QMCPSERVERSTREAMABLEHTTP_H
