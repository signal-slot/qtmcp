// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpclient.h"
#include <QtCore/qcoreapplication.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/private/qfactoryloader_p.h>

#include <QtMcpClient/qmcpclientbackendplugin.h>
#include <QtMcpClient/qmcpclientbackendinterface.h>
#include <QtMcpCommon>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, backendLoader,
                          (QMcpClientBackendPluginFactoryInterface_iid, "/mcpclientbackend"_L1, Qt::CaseInsensitive))

class QMcpClient::Private
{
public:
    QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest; // Default to latest version
    bool tasksExtensionEnabled = false;
    const QList<QtMcp::ProtocolVersion> supportedVersions = {QtMcp::ProtocolVersion::v2024_11_05, QtMcp::ProtocolVersion::v2025_03_26, QtMcp::ProtocolVersion::v2025_06_18, QtMcp::ProtocolVersion::v2025_11_25, QtMcp::ProtocolVersion::v2026_07_28};

    Private(const QString &type, QMcpClient *parent)
        : q(parent)
    {
        backend = qLoadPlugin<QMcpClientBackendInterface, QMcpClientBackendPlugin>(backendLoader(), type);
        if (!backend) {
            qWarning() << type << "not found";
            qWarning() << "call QMcpClient::backends() to get a list of available backends";
            qWarning() << QMcpClient::backends();
            return;
        }

        backend->setParent(q);
        connect(backend, &QMcpClientBackendInterface::started, q, &QMcpClient::started);
        connect(backend, &QMcpClientBackendInterface::errorOccurred, q, &QMcpClient::errorOccurred);
        connect(backend, &QMcpClientBackendInterface::received, q, [this](const QJsonObject &object) {
            if (object.contains("id"_L1)) {
                const auto id = object.value("id"_L1);
                if (object.contains("result"_L1)) {
                    if (callbacks.contains(id)) {
                        const auto result = object.value("result"_L1).toObject();
                        // A multi round-trip interim result (2026-07-28) does
                        // not complete the request; the caller retries with
                        // inputResponses and gets the final result there.
                        if (result.value("resultType"_L1).toString() == "input_required"_L1) {
                            callbacks.remove(id);
                            emit q->inputRequired(id, result);
                            return;
                        }
                        callbacks.take(id)(result, {});
                        return;
                    }
                } else if (object.contains("error"_L1)) {
                    if (callbacks.contains(id)) {
                        const auto error = object.value("error"_L1).toObject();
                        callbacks.take(id)({}, error);
                        return;
                    }
                }
            }
            if (object.contains("method"_L1)) {
                const auto method = object.value("method"_L1).toString();

                // request
                if (object.contains("id"_L1)) {
                    const auto id = object.value("id"_L1);
                    if (requestHandlers.contains(method)) {
                        const auto handler = requestHandlers.value(method);
                        QMcpJSONRPCErrorError error;
                        const auto result = handler(object, &error);
                        if (error.code() != 0) {
                            QMcpJSONRPCError response;
                            response.setId(id.toVariant());
                            // Extract protocol version if available in the request
                            QtMcp::ProtocolVersion reqVersion = protocolVersion;
                            if (object.contains("params"_L1) && object.value("params"_L1).toObject().contains("protocolVersion"_L1)) {
                                QString requestedVersionStr = object.value("params"_L1).toObject().value("protocolVersion"_L1).toString();
                                QtMcp::ProtocolVersion requestedVersion = QtMcp::stringToProtocolVersion(requestedVersionStr);
                                if (supportedVersions.contains(requestedVersion)) {
                                    reqVersion = requestedVersion;
                                }
                            }
                            response.setError(error);
                            // Use the appropriate protocol version for the session
                            q->send(response.toJsonObject(reqVersion));
                        } else {
                            q->send(result);
                        }
                    } else {
                        // Respond with error
                        QMcpJSONRPCError response;
                        response.setId(id.toVariant());
                        auto error = response.error();
                        error.setMessage("Server doesn't handle the request"_L1);
                        response.setError(error);
                        // Use the appropriate protocol version for the session
                        q->send(response.toJsonObject(protocolVersion));
                    }
                    return;
                }

                if (notificationHandlers.contains(method)) {
                    const auto handlers = notificationHandlers.values(method);
                    for (auto &handler : handlers) {
                        handler(object);
                    }
                    return;
                }
            }

            qWarning() << "not handled" << object;
        });
    }

private:
    QMcpClient *q;
public:
    QMcpClientBackendInterface *backend = nullptr;
    QHash<QJsonValue, std::function<void(const QJsonObject &, const QJsonObject &)>> callbacks;
    QHash<QString, std::function<QJsonObject(const QJsonObject &, QMcpJSONRPCErrorError *)>> requestHandlers;
    QMultiHash<QString, std::function<void(const QJsonObject &)>> notificationHandlers;
};

QStringList QMcpClient::backends()
{
    return backendLoader()->keyMap().values();
}

QMcpClient::QMcpClient(const QString &backend, QObject *parent)
    : QObject(parent)
    , d(new Private(backend, this))
{}

QMcpClient::~QMcpClient() = default;

QtMcp::ProtocolVersion QMcpClient::protocolVersion() const
{
    return d->protocolVersion;
}

void QMcpClient::setProtocolVersion(QtMcp::ProtocolVersion protocolVersion)
{
    if (d->protocolVersion == protocolVersion) return;
    d->protocolVersion = protocolVersion;
    emit protocolVersionChanged(protocolVersion);
}


QList<QtMcp::ProtocolVersion> QMcpClient::supportedProtocolVersions() const
{
    return d->supportedVersions;
}

void QMcpClient::start(const QString &args)
{
    if (!d->backend) return;
    d->backend->start(args);
}

void QMcpClient::setTasksExtensionEnabled(bool enabled)
{
    d->tasksExtensionEnabled = enabled;
}

bool QMcpClient::isTasksExtensionEnabled() const
{
    return d->tasksExtensionEnabled;
}

void QMcpClient::send(const QJsonObject &request, std::function<void(const QJsonObject &, const QJsonObject &)> callback)
{
    if (!d->backend) return;

    // If this is an initialization request, ensure the protocol version is set
    if (request.contains("method"_L1) && request.value("method"_L1).toString() == "initialize"_L1) {
        QJsonObject requestCopy = request;
        QJsonObject params = requestCopy.value("params"_L1).toObject();

        // Make sure we're sending our current protocol version
        if (!params.contains("protocolVersion"_L1)) {
            params.insert("protocolVersion"_L1, QtMcp::protocolVersionToString(d->protocolVersion));
            requestCopy.insert("params"_L1, params);
        }

        // Add a callback to handle the initialization response
        auto initCallback = [this, callback](const QJsonObject &result, const QJsonObject &error) {
            if (!error.isEmpty()) {
                // If there was an error, pass it to the original callback
                if (callback)
                    callback(result, error);
                return;
            }

            // Extract and store the protocol version from the server's response
            if (result.contains("protocolVersion"_L1)) {
                QString serverVersionStr = result.value("protocolVersion"_L1).toString();
                // Convert to enum first
                QtMcp::ProtocolVersion serverVersion = QtMcp::stringToProtocolVersion(serverVersionStr);
                if (d->supportedVersions.contains(serverVersion)) {
                    setProtocolVersion(serverVersion);
                    // Let the transport embed the negotiated version where its
                    // protocol requires it, e.g. the MCP-Protocol-Version
                    // header on HTTP (required since 2025-06-18).
                    d->backend->setNegotiatedProtocolVersion(serverVersion);
                } else {
                    // The lifecycle spec says the client SHOULD disconnect
                    // when it cannot support the server's version.
                    qWarning() << "Server negotiated unsupported protocol version"
                               << serverVersionStr << "- disconnecting is recommended";
                }
            }

            // Call the original callback
            if (callback)
                callback(result, error);
        };

        // Send with our wrapped callback
        static int id = 0;
        if (requestCopy.contains("id"_L1) && requestCopy.value("id"_L1).isNull()) {
            auto request2 = requestCopy;
            request2.insert("id"_L1, id);

            d->callbacks.insert(id, initCallback);
            id++;
            d->backend->send(request2);
        } else {
            d->backend->send(requestCopy);
        }

        return;
    }

    // For non-initialization requests, use the standard flow
    auto message = request;

    // Stateless lifecycle (2026-07-28): there is no initialize handshake, so
    // every request identifies the protocol version and the client in its
    // params _meta instead.
    if (d->protocolVersion >= QtMcp::ProtocolVersion::v2026_07_28
        && message.contains("method"_L1) && message.contains("id"_L1)) {
        auto params = message.value("params"_L1).toObject();
        auto meta = params.value("_meta"_L1).toObject();
        meta.insert("io.modelcontextprotocol/protocolVersion"_L1,
                    QtMcp::protocolVersionToString(d->protocolVersion));
        QJsonObject clientInfo;
        clientInfo.insert("name"_L1, QCoreApplication::applicationName());
        clientInfo.insert("version"_L1, QCoreApplication::applicationVersion());
        meta.insert("io.modelcontextprotocol/clientInfo"_L1, clientInfo);
        QJsonObject clientCapabilities;
        if (d->tasksExtensionEnabled) {
            QJsonObject extensions;
            extensions.insert("io.modelcontextprotocol/tasks"_L1, QJsonObject());
            clientCapabilities.insert("extensions"_L1, extensions);
        }
        meta.insert("io.modelcontextprotocol/clientCapabilities"_L1, clientCapabilities);
        params.insert("_meta"_L1, meta);
        message.insert("params"_L1, params);
    }

    static int id = 0;
    if (message.contains("id"_L1) && message.value("id"_L1).isNull()) {
        auto request2 = message;
        request2.insert("id"_L1, id);

        if (callback)
            d->callbacks.insert(id, callback);
        id++;
        d->backend->send(request2);
    } else {
        d->backend->send(message);
    }
}

void QMcpClient::registerRequestHandler(const QString &method, std::function<QJsonObject(const QJsonObject &, QMcpJSONRPCErrorError *)> callback)
{
    qDebug() << method;
    d->requestHandlers.insert(method, callback);
}

void QMcpClient::registerNotificationHandler(const QString &method, std::function<void(const QJsonObject &)> callback)
{
    qDebug() << method;
    d->notificationHandlers.insert(method, callback);
}

QT_END_NAMESPACE
