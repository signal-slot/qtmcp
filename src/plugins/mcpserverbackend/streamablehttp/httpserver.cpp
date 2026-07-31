// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "httpserver.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonValue>
#include <QtCore/QLoggingCategory>
#include <QtCore/QTimer>
#include <QtMcpCommon/qtmcpnamespace.h>

QT_USE_NAMESPACE

using namespace Qt::Literals::StringLiterals;

Q_LOGGING_CATEGORY(lcQMcpServerStreamableHttpPlugin, "qt.mcpserver.plugins.backend.streamablehttp")

namespace {

using HeaderList = QList<std::pair<QByteArray, QByteArray>>;

// An idle SSE stream is kept alive with a comment line at this interval, so
// that proxies and NAT tables do not drop a subscription that has nothing to
// report yet.
constexpr int KeepAliveIntervalMs = 60 * 1000;

// The revision assumed when a request carries no MCP-Protocol-Version header.
// The header only became mandatory in 2025-06-18, so its absence identifies a
// client speaking the revision that introduced Streamable HTTP.
constexpr auto DefaultProtocolVersion = QtMcp::ProtocolVersion::v2025_03_26;

// JSON-RPC error code for a request whose HTTP headers contradict its body.
constexpr int HeaderMismatchErrorCode = -32020;
constexpr int InvalidRequestErrorCode = -32600;
constexpr int ParseErrorCode = -32700;

/*!
    \internal
    Decodes a header value the client wrapped in the \c {=?base64?<data>?=}
    sentinel, which is how values that are not printable ASCII - a tool named
    in Japanese, say - survive an HTTP field. Plain values pass through.
*/
QString decodeHeaderValue(const QByteArray &raw)
{
    const auto trimmed = raw.trimmed();
    if (trimmed.size() > 4 && trimmed.startsWith("=?") && trimmed.endsWith("?=")) {
        const auto inner = trimmed.mid(2, trimmed.size() - 4);
        const auto separator = inner.indexOf('?');
        if (separator > 0 && inner.left(separator).toLower() == "base64") {
            const auto decoded = QByteArray::fromBase64Encoding(inner.mid(separator + 1));
            if (decoded)
                return QString::fromUtf8(*decoded);
            qCWarning(lcQMcpServerStreamableHttpPlugin) << "invalid base64 in header value" << raw;
        }
    }
    return QString::fromUtf8(trimmed);
}

QString headerValue(const QNetworkRequest &request, const QByteArray &name)
{
    if (!request.hasRawHeader(name))
        return {};
    return decodeHeaderValue(request.rawHeader(name));
}

/*!
    \internal
    Returns the value the \c Mcp-Name header has to carry for \a method, or an
    empty string when the method addresses no named entity and therefore must
    not send the header.
*/
QString mcpNameFor(const QString &method, const QJsonObject &params)
{
    if (method == "tools/call"_L1 || method == "prompts/get"_L1)
        return params.value("name"_L1).toString();
    if (method == "resources/read"_L1)
        return params.value("uri"_L1).toString();
    return {};
}

QByteArray jsonRpcErrorBody(const QJsonValue &id, int code, const QString &message)
{
    QJsonObject error;
    error.insert("code"_L1, code);
    error.insert("message"_L1, message);

    QJsonObject response;
    response.insert("jsonrpc"_L1, "2.0"_L1);
    // A message that could not be attributed to a request is answered with a
    // null id, as JSON-RPC requires.
    response.insert("id"_L1, (id.isUndefined() || id.isNull()) ? QJsonValue(QJsonValue::Null) : id);
    response.insert("error"_L1, error);
    return QJsonDocument(response).toJson(QJsonDocument::Compact);
}

QtMcp::ProtocolVersion requestedProtocolVersion(const QNetworkRequest &request, QString *versionString)
{
    auto text = QString::fromUtf8(request.rawHeader("MCP-Protocol-Version")).trimmed();
    if (text.isEmpty())
        text = QtMcp::protocolVersionToString(DefaultProtocolVersion);
    if (versionString)
        *versionString = text;
    return QtMcp::stringToProtocolVersion(text);
}

} // namespace

class HttpServer::Private
{
public:
    Private(HttpServer *parent);

    // A request forwarded to the core, waiting for its response.
    struct Pending {
        QUuid exchange;         // deferResponse() id, i.e. the HTTP connection
        QUuid session;
        QJsonValue originalId;  // the id the client used, restored on the way out
        HeaderList extraHeaders;
        bool stream = false;    // answered as an SSE stream, not as a JSON body
    };

    struct Stream {
        QUuid session;
        bool dedicated = false; // the session exists only for this stream
    };

    struct Session {
        QtMcp::ProtocolVersion version = DefaultProtocolVersion;
        QUuid stream;           // the SSE stream notifications are routed to
    };

    bool isOriginAllowed(const QNetworkRequest &request) const;
    void openStream(const QUuid &streamId, const QUuid &session, bool dedicated);
    void closeStream(const QUuid &streamId);
    // Resolves the session a non-initialize request belongs to. Answers the
    // exchange itself and returns false when it cannot be resolved. A POST
    // that forgot the header is a malformed request (400), while for GET and
    // DELETE there is simply no such session to act on (404).
    bool resolveSession(const QNetworkRequest &request, const QUuid &exchange, QUuid *session,
                        int missingHeaderStatus) const;

    HttpServer *q;
    QStringList allowedOrigins;
    QUuid statelessSession;
    QHash<QUuid, Session> sessions;
    QHash<QString, Pending> pending;   // internal request id -> pending request
    QHash<QUuid, Stream> streams;      // SSE connection id -> stream
    QTimer keepAlive;
    quint64 nextInternalId = 0;
};

HttpServer::Private::Private(HttpServer *parent)
    : q(parent)
{
    keepAlive.setInterval(KeepAliveIntervalMs);
    QObject::connect(&keepAlive, &QTimer::timeout, q, [this]() {
        const auto ids = streams.keys();
        for (const auto &id : ids)
            q->sendSseComment(id, "keep-alive"_ba);
    });
}

bool HttpServer::Private::isOriginAllowed(const QNetworkRequest &request) const
{
    // A request without Origin is not browser initiated, so DNS rebinding does
    // not apply to it.
    if (!request.hasRawHeader("Origin"))
        return true;

    const auto origin = QString::fromUtf8(request.rawHeader("Origin")).trimmed();
    if (origin.startsWith("http://localhost"_L1) || origin.startsWith("http://127.0.0.1"_L1))
        return true;
    return allowedOrigins.contains(origin);
}

void HttpServer::Private::openStream(const QUuid &streamId, const QUuid &session, bool dedicated)
{
    auto it = sessions.find(session);
    if (it != sessions.end()) {
        if (!it->stream.isNull()) {
            qCWarning(lcQMcpServerStreamableHttpPlugin)
                    << "session" << session << "already has a stream; replacing it";
            const auto previous = it->stream;
            closeStream(previous);
            q->closeSseConnection(previous);
            it = sessions.find(session);
        }
        if (it != sessions.end())
            it->stream = streamId;
    }
    streams.insert(streamId, {session, dedicated});
    if (!keepAlive.isActive())
        keepAlive.start();
}

void HttpServer::Private::closeStream(const QUuid &streamId)
{
    if (!streams.contains(streamId))
        return;
    const auto stream = streams.take(streamId);
    if (streams.isEmpty())
        keepAlive.stop();

    auto it = sessions.find(stream.session);
    if (it != sessions.end()) {
        if (it->stream == streamId)
            it->stream = QUuid();
        if (stream.dedicated) {
            // TODO: QMcpServer offers no API to destroy a session, so the core
            // side QMcpServerSession outlives the stream it was created for.
            // Only the transport side mapping is dropped here.
            sessions.erase(it);
        }
    }

    // Drop requests that were being answered through this stream.
    for (auto pendingIt = pending.begin(); pendingIt != pending.end();) {
        if (pendingIt->exchange == streamId)
            pendingIt = pending.erase(pendingIt);
        else
            ++pendingIt;
    }
}

bool HttpServer::Private::resolveSession(const QNetworkRequest &request, const QUuid &exchange,
                                         QUuid *session, int missingHeaderStatus) const
{
    const auto text = QString::fromUtf8(request.rawHeader("Mcp-Session-Id")).trimmed();
    if (text.isEmpty()) {
        q->completeResponse(exchange, missingHeaderStatus,
                            jsonRpcErrorBody({}, InvalidRequestErrorCode,
                                             "Mcp-Session-Id header is required"_L1));
        return false;
    }
    const auto candidate = QUuid::fromString(text);
    if (candidate.isNull() || !sessions.contains(candidate)) {
        q->completeResponse(exchange, 404,
                            jsonRpcErrorBody({}, InvalidRequestErrorCode, "Session not found"_L1));
        return false;
    }
    *session = candidate;
    return true;
}

HttpServer::HttpServer(QObject *parent)
    : QMcpAbstractHttpServer(parent)
    , d(new Private(this))
{
    connect(this, &QMcpAbstractHttpServer::connectionClosed, this, [this](const QUuid &id) {
        if (d->streams.contains(id)) {
            qCDebug(lcQMcpServerStreamableHttpPlugin) << "stream" << id << "closed by the client";
            d->closeStream(id);
            return;
        }
        // Closing the response stream of a request is how a Streamable HTTP
        // client cancels it.
        for (auto it = d->pending.begin(); it != d->pending.end(); ++it) {
            if (it->exchange != id)
                continue;
            // TODO: tell the core to abandon the request. QMcpServer has no API
            // to cancel one in flight, so its response is discarded instead.
            qCDebug(lcQMcpServerStreamableHttpPlugin)
                    << "request" << it.key() << "cancelled by the client";
            d->pending.erase(it);
            return;
        }
    });
}

HttpServer::~HttpServer() = default;

QStringList HttpServer::allowedOrigins() const
{
    return d->allowedOrigins;
}

void HttpServer::setAllowedOrigins(const QStringList &origins)
{
    d->allowedOrigins = origins;
}

void HttpServer::startStatelessSession()
{
    if (!d->statelessSession.isNull())
        return;
    d->statelessSession = QUuid::createUuid();
    d->sessions.insert(d->statelessSession, {QtMcp::ProtocolVersion::v2026_07_28, {}});
    emit newSession(d->statelessSession);
}

QByteArray HttpServer::postMcp(const QNetworkRequest &request, const QByteArray &body)
{
    const auto exchange = deferResponse(request);
    if (exchange.isNull())
        return {};

    if (!d->isOriginAllowed(request)) {
        completeResponse(exchange, 403,
                         jsonRpcErrorBody({}, InvalidRequestErrorCode, "Origin not allowed"_L1));
        return {};
    }

    QString versionString;
    const auto version = requestedProtocolVersion(request, &versionString);

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(body, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        // TODO: 2025-03-26 allows a JSON-RPC batch, i.e. a top level array.
        // Only single messages are accepted for now.
        const QString message = document.isArray()
                ? u"JSON-RPC batches are not supported"_s
                : u"Parse error: %1"_s.arg(parseError.errorString());
        completeResponse(exchange, 400, jsonRpcErrorBody({}, ParseErrorCode, message));
        return {};
    }

    auto message = document.object();
    const auto method = message.value("method"_L1).toString();
    const auto params = message.value("params"_L1).toObject();
    const auto clientId = message.value("id"_L1);
    // JSON-RPC forbids a null id on a request, and the core reads a null id as
    // "allocate one for me", so such a message must not be forwarded as one.
    const bool isRequest = !clientId.isUndefined() && !clientId.isNull() && !method.isEmpty();

    if (version >= QtMcp::ProtocolVersion::v2026_07_28 && !method.isEmpty()) {
        // Since 2026-07-28 the routing information is duplicated into headers
        // so that intermediaries need not parse the body. Any disagreement
        // between the two is a client bug and must not be guessed at.
        const auto metaVersion = params.value("_meta"_L1).toObject()
                                       .value("io.modelcontextprotocol/protocolVersion"_L1).toString();
        // Notifications carry no _meta protocol version, so it is checked when
        // the client sent one and always for requests.
        if ((isRequest || !metaVersion.isEmpty()) && metaVersion != versionString) {
            completeResponse(exchange, 400,
                             jsonRpcErrorBody(clientId, HeaderMismatchErrorCode,
                                              u"MCP-Protocol-Version header '%1' does not match the "
                                              "params._meta protocol version '%2'"_s
                                              .arg(versionString, metaVersion)));
            return {};
        }

        if (!request.hasRawHeader("Mcp-Method")) {
            completeResponse(exchange, 400,
                             jsonRpcErrorBody(clientId, HeaderMismatchErrorCode,
                                              "Mcp-Method header is required since 2026-07-28"_L1));
            return {};
        }
        const auto headerMethod = headerValue(request, "Mcp-Method");
        if (headerMethod != method) {
            completeResponse(exchange, 400,
                             jsonRpcErrorBody(clientId, HeaderMismatchErrorCode,
                                              u"Mcp-Method header '%1' does not match the method "
                                              "'%2' in the body"_s.arg(headerMethod, method)));
            return {};
        }

        const auto expectedName = mcpNameFor(method, params);
        if (!expectedName.isEmpty()) {
            if (!request.hasRawHeader("Mcp-Name")) {
                completeResponse(exchange, 400,
                                 jsonRpcErrorBody(clientId, HeaderMismatchErrorCode,
                                                  u"Mcp-Name header is required for '%1'"_s.arg(method)));
                return {};
            }
            const auto headerName = headerValue(request, "Mcp-Name");
            if (headerName != expectedName) {
                completeResponse(exchange, 400,
                                 jsonRpcErrorBody(clientId, HeaderMismatchErrorCode,
                                                  u"Mcp-Name header '%1' does not match '%2' in the "
                                                  "body"_s.arg(headerName, expectedName)));
                return {};
            }
        }
        // TODO: Mcp-Param-* headers, which mirror x-mcp-header annotated tool
        // arguments, are accepted but not validated against the body.
    }

    // Resolve, or create, the session this message belongs to.
    QUuid session;
    HeaderList extraHeaders;
    if (version >= QtMcp::ProtocolVersion::v2026_07_28) {
        // 2026-07-28 dropped Mcp-Session-Id; a value sent anyway is ignored
        // deliberately rather than rejected.
        if (method == "subscriptions/listen"_L1) {
            // The core routes notifications per session, so a subscription
            // needs a session of its own to keep its stream apart from the
            // shared stateless one.
            session = QUuid::createUuid();
            d->sessions.insert(session, {version, {}});
            emit newSession(session);
        } else {
            if (d->statelessSession.isNull())
                startStatelessSession();
            session = d->statelessSession;
        }
    } else if (method == "initialize"_L1) {
        session = QUuid::createUuid();
        d->sessions.insert(session, {version, {}});
        emit newSession(session);
        extraHeaders.append({"Mcp-Session-Id"_ba, session.toByteArray(QUuid::WithoutBraces)});
    } else if (!d->resolveSession(request, exchange, &session, 400)) {
        return {};
    }

    if (!isRequest) {
        // A notification, or a response to a server initiated request. Neither
        // produces a body, so the exchange is finished before the core runs and
        // possibly emits notifications of its own.
        completeResponse(exchange, 202, {}, QStringLiteral("application/json"), extraHeaders);
        emit received(session, message);
        return {};
    }

    // Several clients share the stateless session and may pick the same request
    // id, so the id handed to the core is rewritten to a unique one and
    // restored when the response comes back.
    const auto internalId = u"qtmcp-%1"_s.arg(d->nextInternalId++);
    message.insert("id"_L1, internalId);

    Private::Pending entry;
    entry.exchange = exchange;
    entry.session = session;
    entry.originalId = clientId;
    entry.extraHeaders = extraHeaders;

    if (method == "subscriptions/listen"_L1) {
        // The response to a subscription is the stream itself. It has to be
        // opened before the core handles the request, because the handler
        // acknowledges the subscription through that very stream.
        if (!upgradeToSse(exchange, extraHeaders)) {
            completeResponse(exchange, 500,
                             jsonRpcErrorBody(clientId, InvalidRequestErrorCode,
                                              "Failed to open the subscription stream"_L1));
            return {};
        }
        entry.stream = true;
        d->openStream(exchange, session, version >= QtMcp::ProtocolVersion::v2026_07_28);
    }

    d->pending.insert(internalId, entry);
    emit received(session, message);
    return {};
}

QByteArray HttpServer::getMcp(const QNetworkRequest &request)
{
    const auto exchange = deferResponse(request);
    if (exchange.isNull())
        return {};

    if (!d->isOriginAllowed(request)) {
        completeResponse(exchange, 403,
                         jsonRpcErrorBody({}, InvalidRequestErrorCode, "Origin not allowed"_L1));
        return {};
    }

    if (requestedProtocolVersion(request, nullptr) >= QtMcp::ProtocolVersion::v2026_07_28) {
        // 2026-07-28 replaced the standing GET stream with subscriptions/listen.
        completeResponse(exchange, 405,
                         jsonRpcErrorBody({}, InvalidRequestErrorCode,
                                          "GET was removed in MCP 2026-07-28; use "
                                          "subscriptions/listen instead"_L1));
        return {};
    }

    if (!QString::fromUtf8(request.rawHeader("Accept")).contains("text/event-stream"_L1)) {
        completeResponse(exchange, 405,
                         jsonRpcErrorBody({}, InvalidRequestErrorCode,
                                          "Accept: text/event-stream is required"_L1));
        return {};
    }

    QUuid session;
    if (!d->resolveSession(request, exchange, &session, 404))
        return {};

    // TODO: resuming a stream with Last-Event-ID is not implemented, so the
    // header is ignored and the client always gets a fresh stream.
    if (!upgradeToSse(exchange))
        return {};
    d->openStream(exchange, session, false);
    qCDebug(lcQMcpServerStreamableHttpPlugin) << "standalone stream opened for session" << session;
    return {};
}

QByteArray HttpServer::deleteMcp(const QNetworkRequest &request)
{
    const auto exchange = deferResponse(request);
    if (exchange.isNull())
        return {};

    if (!d->isOriginAllowed(request)) {
        completeResponse(exchange, 403,
                         jsonRpcErrorBody({}, InvalidRequestErrorCode, "Origin not allowed"_L1));
        return {};
    }

    if (requestedProtocolVersion(request, nullptr) >= QtMcp::ProtocolVersion::v2026_07_28) {
        // Without a session there is nothing to terminate.
        completeResponse(exchange, 405,
                         jsonRpcErrorBody({}, InvalidRequestErrorCode,
                                          "DELETE was removed in MCP 2026-07-28"_L1));
        return {};
    }

    QUuid session;
    if (!d->resolveSession(request, exchange, &session, 404))
        return {};

    const auto stream = d->sessions.value(session).stream;
    if (!stream.isNull()) {
        d->closeStream(stream);
        closeSseConnection(stream);
    }
    // TODO: QMcpServer offers no API to destroy a session, so its
    // QMcpServerSession stays alive until the server shuts down.
    d->sessions.remove(session);
    completeResponse(exchange, 200);
    qCDebug(lcQMcpServerStreamableHttpPlugin) << "session" << session << "terminated";
    return {};
}

void HttpServer::send(const QUuid &session, const QJsonObject &object)
{
    // A response to a request forwarded earlier carries the internal id and no
    // method; a server initiated request carries both.
    if (object.contains("id"_L1) && !object.contains("method"_L1)) {
        const auto internalId = object.value("id"_L1).toString();
        if (d->pending.contains(internalId)) {
            const auto entry = d->pending.take(internalId);
            if (entry.stream) {
                // TODO: the JSON-RPC response to subscriptions/listen signals a
                // graceful end of the subscription and should close the stream.
                // The core answers it immediately, so honouring that here would
                // end every subscription at once. The stream is kept open and
                // torn down when the client disconnects instead.
                qCDebug(lcQMcpServerStreamableHttpPlugin)
                        << "keeping the subscription stream open for" << entry.session;
                return;
            }

            auto response = object;
            if (entry.originalId.isUndefined())
                response.remove("id"_L1);
            else
                response.insert("id"_L1, entry.originalId);
            // TODO: 2026-07-28 wants a -32601 from the core mapped to HTTP 404.
            // Every JSON-RPC error is reported as 200 with an error body here.
            completeResponse(entry.exchange, 200,
                             QJsonDocument(response).toJson(QJsonDocument::Compact),
                             QStringLiteral("application/json"), entry.extraHeaders);
            return;
        }
    }

    // Anything else - notifications and server initiated requests - belongs on
    // the session's stream.
    const auto it = d->sessions.constFind(session);
    if (it != d->sessions.constEnd() && !it->stream.isNull()) {
        sendSseEvent(it->stream, QJsonDocument(object).toJson(QJsonDocument::Compact));
        return;
    }

    qCWarning(lcQMcpServerStreamableHttpPlugin)
            << "session" << session << "has no open stream; dropping" << object;
}
