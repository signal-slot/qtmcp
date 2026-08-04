// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpclientstreamablehttp.h"

#include <cmath>
#include <functional>
#include <memory>
#include <optional>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QLoggingCategory>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtNetwork/QNetworkRequest>

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

Q_LOGGING_CATEGORY(lcQMcpClientStreamableHttpPlugin, "qt.mcpclient.plugins.backend.streamablehttp")

namespace {

// The GET stream is reopened this long after the server closed it. The spec
// allows resuming with Last-Event-ID; that is not implemented yet, so a
// reconnect always starts a fresh stream.
// TODO: honour the SSE id field and resume with Last-Event-ID.
constexpr int ServerStreamReconnectIntervalMs = 5000;

// Sentinel wrapper for header values that cannot be written as plain ASCII,
// e.g. a tool named in Japanese: "=?base64?<base64 of the UTF-8 value>?=".
constexpr auto HeaderEncodingPrefix = "=?base64?"_L1;
constexpr auto HeaderEncodingSuffix = "?="_L1;

/*!
    \internal
    Returns true if \a c is an RFC 9110 token character, i.e. valid in an HTTP
    field name.
*/
bool isTokenChar(QChar c)
{
    if (c.unicode() > 0x7e)
        return false;
    if ((c >= u'0' && c <= u'9') || (c >= u'a' && c <= u'z') || (c >= u'A' && c <= u'Z'))
        return true;
    return "!#$%&'*+-.^_`|~"_L1.contains(c);
}

bool isToken(const QString &text)
{
    if (text.isEmpty())
        return false;
    for (const QChar c : text) {
        if (!isTokenChar(c))
            return false;
    }
    return true;
}

/*!
    \internal
    Encodes \a value for transport in an HTTP field value. Values consisting of
    visible ASCII only are sent verbatim; anything else - and any value that
    would be mistaken for the sentinel itself - is base64 encoded.
*/
QByteArray encodeHeaderValue(const QString &value)
{
    bool needsEncoding = value.startsWith(HeaderEncodingPrefix) && value.endsWith(HeaderEncodingSuffix);
    if (!needsEncoding) {
        for (const QChar c : value) {
            if (c.unicode() < 0x21 || c.unicode() > 0x7e) {
                needsEncoding = true;
                break;
            }
        }
    }
    if (!needsEncoding)
        return value.toLatin1();
    return QByteArray("=?base64?") + value.toUtf8().toBase64() + QByteArray("?=");
}

/*!
    \internal
    Incremental Server-Sent Events parser. Chunks are appended as they arrive
    and every complete event is reported through the callback passed to
    append(), carrying the concatenated payload of its \c data: fields.
*/
class SseParser
{
public:
    void append(const QByteArray &chunk, const std::function<void(const QByteArray &)> &onEvent)
    {
        buffer.append(chunk);
        while (true) {
            // The separator is either CRLF or LF; determine it from the first
            // complete event and stick to it for the rest of the stream.
            if (separator.isEmpty()) {
                if (buffer.contains("\r\n\r\n")) {
                    separator = "\r\n";
                    terminator = "\r\n\r\n";
                } else if (buffer.contains("\n\n")) {
                    separator = "\n";
                    terminator = "\n\n";
                } else {
                    break;
                }
            }
            const auto end = buffer.indexOf(terminator);
            if (end < 0)
                break;

            const auto event = buffer.left(end);
            buffer.remove(0, end + terminator.length());

            QByteArray payload;
            qsizetype from = 0;
            while (from <= event.length()) {
                auto next = event.indexOf(separator, from);
                if (next < 0)
                    next = event.length();
                const auto line = event.mid(from, next - from);
                from = next + separator.length();

                // Comments (": ping" and friends) and fields other than data
                // carry no JSON-RPC payload.
                if (!line.startsWith("data:"))
                    continue;
                auto data = line.mid(5);
                if (data.startsWith(' '))
                    data.remove(0, 1);
                if (!payload.isEmpty())
                    payload.append('\n');
                payload.append(data);
            }

            if (!payload.isEmpty())
                onEvent(payload);
        }
    }

private:
    QByteArray buffer;
    QByteArray separator;
    QByteArray terminator; // separator twice, i.e. the end of one event
};

/*!
    \internal
    Returns the value the Mcp-Name header must carry for \a method, or an empty
    string when the method addresses no named entity.
*/
QString mcpNameFor(const QString &method, const QJsonObject &params)
{
    if (method == "tools/call"_L1 || method == "prompts/get"_L1)
        return params.value("name"_L1).toString();
    if (method == "resources/read"_L1)
        return params.value("uri"_L1).toString();
    return {};
}

} // namespace

class QMcpClientStreamableHttp::Private
{
public:
    Private(QMcpClientStreamableHttp *parent);

    void start(const QUrl &url);
    void post(const QJsonObject &object);
    void setNegotiatedProtocolVersion(QtMcp::ProtocolVersion protocolVersion);

private:
    bool isStateless() const
    {
        return negotiatedProtocolVersion
                && *negotiatedProtocolVersion >= QtMcp::ProtocolVersion::v2026_07_28;
    }

    QNetworkRequest createRequest(const QJsonObject &object) const;
    void applyToolCallHeaders(QNetworkRequest &request, const QJsonObject &params) const;
    void ignoreSslErrors(QNetworkReply *reply) const;
    void storeSessionId(QNetworkReply *reply);
    void reportHttpError(QNetworkReply *reply, int statusCode, const QByteArray &body);
    void dispatch(const QByteArray &payload);
    void emitReceived(const QJsonObject &object);
    void cacheToolHeaderAnnotations(const QJsonObject &object);
    void openServerStream();

    QMcpClientStreamableHttp *q;
    QUrl endpoint;
    QNetworkAccessManager networkAccessManager;
    std::optional<QtMcp::ProtocolVersion> negotiatedProtocolVersion;
    QByteArray sessionId;
    QScopedPointer<QNetworkReply> serverStream;
    bool serverStreamRejected = false;
    // tool name -> (argument property name -> header name suffix)
    QHash<QString, QHash<QString, QString>> toolHeaderAnnotations;
};

QMcpClientStreamableHttp::Private::Private(QMcpClientStreamableHttp *parent)
    : q(parent)
{}

void QMcpClientStreamableHttp::Private::start(const QUrl &url)
{
    endpoint = url;
    // A bare host such as "http://localhost:8000" addresses the conventional
    // /mcp endpoint; an explicit path is taken as given.
    if (endpoint.path().isEmpty())
        endpoint.setPath("/mcp"_L1);

    // Streamable HTTP has no connection handshake, so the backend is usable as
    // soon as the endpoint is known. started() is emitted asynchronously so
    // callers can connect to it after calling start().
    QMetaObject::invokeMethod(q, [this]() {
        emit q->started();
    }, Qt::QueuedConnection);
}

QNetworkRequest QMcpClientStreamableHttp::Private::createRequest(const QJsonObject &object) const
{
    QNetworkRequest request(endpoint);
    // The server picks the response format, so both must be accepted.
    request.setRawHeader("Accept", "application/json, text/event-stream");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Required on every request once negotiation completed; before that the
    // version is not known yet (the initialize request itself carries it in
    // its body).
    if (negotiatedProtocolVersion) {
        request.setRawHeader("MCP-Protocol-Version",
                             QtMcp::protocolVersionToString(*negotiatedProtocolVersion).toLatin1());
    }
    if (!sessionId.isEmpty())
        request.setRawHeader("Mcp-Session-Id", sessionId);

    if (isStateless()) {
        // 2026-07-28 requires the routing headers so that intermediaries can
        // dispatch without parsing the body.
        const auto method = object.value("method"_L1).toString();
        // Responses to server initiated requests carry no method.
        if (!method.isEmpty()) {
            request.setRawHeader("Mcp-Method", encodeHeaderValue(method));
            const auto params = object.value("params"_L1).toObject();
            const auto name = mcpNameFor(method, params);
            if (!name.isEmpty())
                request.setRawHeader("Mcp-Name", encodeHeaderValue(name));
            if (method == "tools/call"_L1)
                applyToolCallHeaders(request, params);
        }
    }

    return request;
}

void QMcpClientStreamableHttp::Private::applyToolCallHeaders(QNetworkRequest &request, const QJsonObject &params) const
{
    const auto annotations = toolHeaderAnnotations.value(params.value("name"_L1).toString());
    if (annotations.isEmpty())
        return;

    const auto arguments = params.value("arguments"_L1).toObject();
    for (auto it = annotations.cbegin(), end = annotations.cend(); it != end; ++it) {
        const auto value = arguments.value(it.key());
        if (value.isUndefined() || value.isNull())
            continue;

        QString text;
        if (value.isString()) {
            text = value.toString();
        } else if (value.isBool()) {
            text = value.toBool() ? u"true"_s : u"false"_s;
        } else if (value.isDouble()) {
            const double number = value.toDouble();
            if (number != std::trunc(number)) {
                qCWarning(lcQMcpClientStreamableHttpPlugin)
                        << "x-mcp-header on a fractional number is not mirrorable; skipping"
                        << it.key();
                continue;
            }
            text = QString::number(value.toInteger());
        } else {
            qCWarning(lcQMcpClientStreamableHttpPlugin)
                    << "x-mcp-header supports strings, integers and booleans only; skipping"
                    << it.key();
            continue;
        }

        request.setRawHeader("Mcp-Param-" + it.value().toLatin1(), encodeHeaderValue(text));
    }
}

void QMcpClientStreamableHttp::Private::ignoreSslErrors(QNetworkReply *reply) const
{
    connect(reply, &QNetworkReply::sslErrors, q, [reply](const QList<QSslError> &errors) {
        for (const QSslError &error : errors)
            qCDebug(lcQMcpClientStreamableHttpPlugin) << error.errorString();
        reply->ignoreSslErrors();
    });
}

void QMcpClientStreamableHttp::Private::storeSessionId(QNetworkReply *reply)
{
    const auto id = reply->rawHeader("Mcp-Session-Id");
    if (id.isEmpty())
        return;
    sessionId = id;
    qCDebug(lcQMcpClientStreamableHttpPlugin) << "session established" << sessionId;
}

void QMcpClientStreamableHttp::Private::reportHttpError(QNetworkReply *reply, int statusCode, const QByteArray &body)
{
    const auto reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    QString message = QStringLiteral("HTTP %1 %2").arg(statusCode).arg(reason);
    if (statusCode == 404 && !sessionId.isEmpty()) {
        // The server no longer knows this session; the application has to run
        // the initialize handshake again to obtain a new one.
        sessionId.clear();
        message += QStringLiteral(": the session expired, initialize again");
    }
    if (!body.isEmpty())
        message += QStringLiteral(": ") + QString::fromUtf8(body).simplified();
    qCWarning(lcQMcpClientStreamableHttpPlugin) << message;
    emit q->errorOccurred(message);
}

void QMcpClientStreamableHttp::Private::dispatch(const QByteArray &payload)
{
    QJsonParseError error;
    const auto json = QJsonDocument::fromJson(payload, &error);
    if (error.error) {
        qCWarning(lcQMcpClientStreamableHttpPlugin) << error.errorString() << payload;
        return;
    }

    if (json.isArray()) {
        // Batched JSON-RPC, as used by 2025-03-26.
        const auto array = json.array();
        for (const auto &value : array) {
            if (value.isObject())
                emitReceived(value.toObject());
            else
                qCWarning(lcQMcpClientStreamableHttpPlugin) << "unexpected batch entry" << value;
        }
    } else if (json.isObject()) {
        emitReceived(json.object());
    } else {
        qCWarning(lcQMcpClientStreamableHttpPlugin) << "unexpected payload" << payload;
    }
}

void QMcpClientStreamableHttp::Private::emitReceived(const QJsonObject &object)
{
    cacheToolHeaderAnnotations(object);
    emit q->received(object);
}

void QMcpClientStreamableHttp::Private::cacheToolHeaderAnnotations(const QJsonObject &object)
{
    const auto result = object.value("result"_L1).toObject();
    const auto tools = result.value("tools"_L1);
    if (!tools.isArray())
        return;

    const auto array = tools.toArray();
    for (const auto &value : array) {
        const auto tool = value.toObject();
        const auto toolName = tool.value("name"_L1).toString();
        if (toolName.isEmpty())
            continue;

        const auto inputSchema = tool.value("inputSchema"_L1).toObject();
        QHash<QString, QString> annotations;

        // The annotation is only meaningful on a schema reached through a
        // chain of "properties"; anywhere else it must be ignored.
        // TODO: reject the whole tool instead of only ignoring the annotation,
        // which is what the spec mandates.
        if (inputSchema.contains("x-mcp-header"_L1)) {
            qCWarning(lcQMcpClientStreamableHttpPlugin)
                    << "ignoring x-mcp-header on the input schema root of" << toolName;
        }

        const auto properties = inputSchema.value("properties"_L1).toObject();
        for (auto it = properties.constBegin(), end = properties.constEnd(); it != end; ++it) {
            const auto property = it.value().toObject();

            // TODO: support nested properties chains; only top level argument
            // properties are mirrored for now.
            const auto nested = property.value("properties"_L1).toObject();
            for (const auto &child : nested) {
                if (child.toObject().contains("x-mcp-header"_L1)) {
                    qCWarning(lcQMcpClientStreamableHttpPlugin)
                            << "nested x-mcp-header is not supported yet; ignoring it under"
                            << toolName << it.key();
                    break;
                }
            }
            if (property.value("items"_L1).toObject().contains("x-mcp-header"_L1)) {
                qCWarning(lcQMcpClientStreamableHttpPlugin)
                        << "ignoring x-mcp-header outside a properties chain in"
                        << toolName << it.key();
            }

            const auto annotation = property.value("x-mcp-header"_L1);
            if (annotation.isUndefined())
                continue;
            if (!annotation.isString()) {
                qCWarning(lcQMcpClientStreamableHttpPlugin)
                        << "ignoring non string x-mcp-header in" << toolName << it.key();
                continue;
            }

            const auto headerName = annotation.toString();
            if (!isToken(headerName)) {
                qCWarning(lcQMcpClientStreamableHttpPlugin)
                        << "ignoring x-mcp-header that is not a valid header name in"
                        << toolName << it.key() << headerName;
                continue;
            }
            // Fractional values have no canonical header representation, so
            // the annotation is invalid on "number" typed properties.
            if (property.value("type"_L1).toString() == "number"_L1) {
                qCWarning(lcQMcpClientStreamableHttpPlugin)
                        << "ignoring x-mcp-header on a number property in"
                        << toolName << it.key();
                continue;
            }

            annotations.insert(it.key(), headerName);
        }

        if (annotations.isEmpty())
            toolHeaderAnnotations.remove(toolName);
        else
            toolHeaderAnnotations.insert(toolName, annotations);
    }
}

void QMcpClientStreamableHttp::Private::post(const QJsonObject &object)
{
    if (endpoint.isEmpty()) {
        qCWarning(lcQMcpClientStreamableHttpPlugin) << "the backend has not been started yet";
        return;
    }

    const bool initialize = object.value("method"_L1).toString() == "initialize"_L1;
    const auto request = createRequest(object);
    const auto data = QJsonDocument(object).toJson(QJsonDocument::Compact);
    qCDebug(lcQMcpClientStreamableHttpPlugin) << data;

    auto *reply = networkAccessManager.post(request, data);
    ignoreSslErrors(reply);

    // The response is either a single JSON object or an SSE stream that stays
    // open until the server has sent everything it owes for this request.
    // TODO: subscriptions/listen streams are not reopened automatically when
    // the server closes them; resubscribing is left to the application.
    struct ReplyState {
        SseParser parser;
        QByteArray body;
    };
    auto state = std::make_shared<ReplyState>();

    if (initialize) {
        // The session id has to be known before the initialize result reaches
        // the client, because the client answers it by negotiating the
        // protocol version, which may open the GET stream right away.
        connect(reply, &QNetworkReply::metaDataChanged, q, [this, reply]() {
            storeSessionId(reply);
        });
    }

    connect(reply, &QNetworkReply::readyRead, q, [this, reply, state]() {
        const auto contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        const auto chunk = reply->readAll();
        qCDebug(lcQMcpClientStreamableHttpPlugin) << chunk;
        if (contentType.startsWith("text/event-stream"_L1)) {
            state->parser.append(chunk, [this](const QByteArray &payload) {
                dispatch(payload);
            });
        } else {
            state->body.append(chunk);
        }
    });

    connect(reply, &QNetworkReply::finished, q, [this, reply, state, initialize]() {
        reply->deleteLater();
        if (initialize)
            storeSessionId(reply);

        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 0) {
            // The request never reached the server.
            qCWarning(lcQMcpClientStreamableHttpPlugin) << reply->errorString();
            emit q->errorOccurred(reply->errorString());
            return;
        }
        if (statusCode >= 400) {
            reportHttpError(reply, statusCode, state->body);
            return;
        }
        // 202 Accepted acknowledges a notification and carries no body.
        if (!state->body.isEmpty())
            dispatch(state->body);
    });
}

void QMcpClientStreamableHttp::Private::openServerStream()
{
    if (endpoint.isEmpty() || serverStreamRejected)
        return;
    // 2026-07-28 replaced the standing GET stream with subscriptions/listen.
    if (!negotiatedProtocolVersion || isStateless())
        return;
    if (!serverStream.isNull() && serverStream->isRunning())
        return;

    QNetworkRequest request(endpoint);
    request.setRawHeader("Accept", "text/event-stream");
    request.setRawHeader("Cache-Control", "no-cache");
    request.setRawHeader("MCP-Protocol-Version",
                         QtMcp::protocolVersionToString(*negotiatedProtocolVersion).toLatin1());
    if (!sessionId.isEmpty())
        request.setRawHeader("Mcp-Session-Id", sessionId);

    auto *reply = networkAccessManager.get(request);
    serverStream.reset(reply);
    ignoreSslErrors(reply);

    auto parser = std::make_shared<SseParser>();
    connect(reply, &QNetworkReply::readyRead, q, [this, reply, parser]() {
        const auto chunk = reply->readAll();
        qCDebug(lcQMcpClientStreamableHttpPlugin) << chunk;
        parser->append(chunk, [this](const QByteArray &payload) {
            dispatch(payload);
        });
    });

    connect(reply, &QNetworkReply::finished, q, [this, reply]() {
        const auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 405) {
            // The server does not offer a GET stream, which the spec allows.
            qCDebug(lcQMcpClientStreamableHttpPlugin) << "the server declined the GET stream";
            serverStreamRejected = true;
            return;
        }
        if (statusCode >= 400) {
            qCWarning(lcQMcpClientStreamableHttpPlugin)
                    << "the GET stream failed with HTTP" << statusCode;
        }
        // The reply is owned by serverStream and is destroyed once the next
        // stream replaces it, so it must not be deleted from its own slot.
        QTimer::singleShot(ServerStreamReconnectIntervalMs, q, [this]() {
            openServerStream();
        });
    });
}

void QMcpClientStreamableHttp::Private::setNegotiatedProtocolVersion(QtMcp::ProtocolVersion protocolVersion)
{
    negotiatedProtocolVersion = protocolVersion;
    openServerStream();
}

QMcpClientStreamableHttp::QMcpClientStreamableHttp(QObject *parent)
    : QMcpClientBackendInterface(parent)
    , d(new Private(this))
{}

QMcpClientStreamableHttp::~QMcpClientStreamableHttp() = default;

void QMcpClientStreamableHttp::start(const QString &server)
{
    d->start(QUrl(server));
}

void QMcpClientStreamableHttp::send(const QJsonObject &object)
{
    d->post(object);
}

void QMcpClientStreamableHttp::notify(const QJsonObject &object)
{
    // Notifications travel the same POST path; the server answers 202.
    d->post(object);
}

void QMcpClientStreamableHttp::setNegotiatedProtocolVersion(QtMcp::ProtocolVersion protocolVersion)
{
    d->setNegotiatedProtocolVersion(protocolVersion);
}

QT_END_NAMESPACE
