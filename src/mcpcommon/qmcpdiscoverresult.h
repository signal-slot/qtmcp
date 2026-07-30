// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPDISCOVERRESULT_H
#define QMCPDISCOVERRESULT_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpcacheableresult.h>
#include <QtMcpCommon/qmcpservercapabilities.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpDiscoverResult
    \inmodule QtMcpCommon
    \brief The result returned by the server for a server/discover request.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpDiscoverResult : public QMcpCacheableResult
{
    Q_GADGET

    /*!
        \property QMcpDiscoverResult::capabilities
        \brief The capabilities of the server.
    */
    Q_PROPERTY(QMcpServerCapabilities capabilities READ capabilities WRITE setCapabilities REQUIRED)

    /*!
        \property QMcpDiscoverResult::instructions
        \brief Natural-language guidance describing the server and its features.

        This can be used by clients to improve an LLM's understanding of
        available tools, for example by including it in a system prompt. It
        should focus on information that helps the model use the server
        effectively and should not duplicate tool descriptions.
    */
    Q_PROPERTY(QString instructions READ instructions WRITE setInstructions)

    /*!
        \property QMcpDiscoverResult::supportedVersions
        \brief MCP protocol versions this server supports.

        The client should pick one of these versions for subsequent requests.
    */
    Q_PROPERTY(QList<QString> supportedVersions READ supportedVersions WRITE setSupportedVersions REQUIRED)

public:
    QMcpDiscoverResult() : QMcpCacheableResult(new Private) {}

    QMcpServerCapabilities capabilities() const {
        return d<Private>()->capabilities;
    }

    void setCapabilities(const QMcpServerCapabilities &capabilities) {
        if (this->capabilities() == capabilities) return;
        d<Private>()->capabilities = capabilities;
    }

    QString instructions() const {
        return d<Private>()->instructions;
    }

    void setInstructions(const QString &instructions) {
        if (this->instructions() == instructions) return;
        d<Private>()->instructions = instructions;
    }

    QList<QString> supportedVersions() const {
        return d<Private>()->supportedVersions;
    }

    void setSupportedVersions(const QList<QString> &supportedVersions) {
        if (this->supportedVersions() == supportedVersions) return;
        d<Private>()->supportedVersions = supportedVersions;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpCacheableResult::Private {
        QMcpServerCapabilities capabilities;
        QString instructions;
        QList<QString> supportedVersions;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpDiscoverResult)

QT_END_NAMESPACE

#endif // QMCPDISCOVERRESULT_H
