// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTSTREAMABLEHTTP_H
#define QMCPCLIENTSTREAMABLEHTTP_H

#include <QtMcpClient/qmcpclientbackendinterface.h>
#include <QtMcpClient/qmcpclientbackendplugin.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

QT_BEGIN_NAMESPACE

/*!
    \class QMcpClientStreamableHttp
    \internal
    \brief Client backend for the MCP Streamable HTTP transport.

    Every JSON-RPC message is sent as an HTTP POST to a single endpoint. The
    server answers either with \c 202 (notification accepted), a single JSON
    object, or an SSE stream carrying one or more JSON-RPC messages.

    Protocol revisions before 2026-07-28 additionally allow the client to open a
    standing GET SSE stream for server-initiated requests and notifications.
    2026-07-28 removed that stream in favour of \c subscriptions/listen, whose
    response is itself an SSE stream and therefore travels the POST path.
*/
class QMcpClientStreamableHttp : public QMcpClientBackendInterface
{
    Q_OBJECT
public:
    explicit QMcpClientStreamableHttp(QObject *parent = nullptr);
    ~QMcpClientStreamableHttp() override;

public slots:
    void start(const QString &server) override;
    void send(const QJsonObject &object) override;
    void notify(const QJsonObject &object) override;
    void setNegotiatedProtocolVersion(QtMcp::ProtocolVersion protocolVersion) override;

private:
    class Private;
    QScopedPointer<Private> d;
};

class QMcpClientStreamableHttpPlugin : public QMcpClientBackendPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QMcpClientBackendPluginFactoryInterface_iid FILE "qmcpclientstreamablehttp.json")
public:
    QMcpClientBackendInterface *create(const QString &key, QObject *parent) override
    {
        static const QStringList keys { "streamablehttp"_L1 };
        Q_ASSERT(keys.contains(key));
        return new QMcpClientStreamableHttp(parent);
    }
};

QT_END_NAMESPACE

#endif // QMCPCLIENTSTREAMABLEHTTP_H
