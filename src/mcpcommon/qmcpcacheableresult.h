// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCACHEABLERESULT_H
#define QMCPCACHEABLERESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCacheableResult
    \inmodule QtMcpCommon
    \brief A result that supports a time-to-live (TTL) hint for client-side caching.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpCacheableResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpCacheableResult::cacheScope
        \brief The intended scope of the cached response, analogous to HTTP
        \c {Cache-Control: public} vs \c {Cache-Control: private}.

        \c "public" means the response holds no user specific data and may be
        cached by any intermediary, \c "private" restricts reuse to the same
        authorization context. The default is \c "private".

        Required on the wire since MCP 2026-07-28; earlier revisions never
        serialize it.
        \since MCP 2026-07-28
    */
    Q_PROPERTY(QString cacheScope READ cacheScope WRITE setCacheScope REQUIRED)

    /*!
        \property QMcpCacheableResult::ttlMs
        \brief How long, in milliseconds, the client may cache this response
        before re-fetching it.

        0, the default, marks the response as immediately stale. A positive
        value keeps it fresh for that many milliseconds after reception.

        Required on the wire since MCP 2026-07-28; earlier revisions never
        serialize it.
        \since MCP 2026-07-28
    */
    Q_PROPERTY(int ttlMs READ ttlMs WRITE setTtlMs REQUIRED)

public:
    QMcpCacheableResult() : QMcpResult(new Private) {}
protected:
    QMcpCacheableResult(Private *d) : QMcpResult(d) {}
public:

    QString cacheScope() const {
        return d<Private>()->cacheScope;
    }

    void setCacheScope(const QString &cacheScope) {
        if (this->cacheScope() == cacheScope) return;
        d<Private>()->cacheScope = cacheScope;
    }

    int ttlMs() const {
        return d<Private>()->ttlMs;
    }

    void setTtlMs(int ttlMs) {
        if (this->ttlMs() == ttlMs) return;
        d<Private>()->ttlMs = ttlMs;
    }

    bool fromJsonObject(const QJsonObject &object, QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest) override {
        // Both members are mandatory on 2026-07-28, but a server omitting them
        // is not a reason to reject the whole result: fall back to the
        // "do not cache" defaults instead of failing the REQUIRED check.
        if (protocolVersion >= QtMcp::ProtocolVersion::v2026_07_28
            && (!object.contains("cacheScope"_L1) || !object.contains("ttlMs"_L1))) {
            auto copy = object;
            if (!copy.contains("cacheScope"_L1))
                copy.insert("cacheScope"_L1, "private"_L1);
            if (!copy.contains("ttlMs"_L1))
                copy.insert("ttlMs"_L1, 0);
            return QMcpResult::fromJsonObject(copy, protocolVersion);
        }
        return QMcpResult::fromJsonObject(object, protocolVersion);
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "cacheScope" || name == "ttlMs")
            return protocolVersion >= QtMcp::ProtocolVersion::v2026_07_28;
        return QMcpResult::isPropertyAvailable(name, protocolVersion);
    }

    struct Private : public QMcpResult::Private {
        QString cacheScope = QStringLiteral("private");
        int ttlMs = 0;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCACHEABLERESULT_H
