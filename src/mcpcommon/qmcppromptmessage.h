// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROMPTMESSAGE_H
#define QMCPPROMPTMESSAGE_H

#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcppromptmessagecontent.h>
#include <QtMcpCommon/qmcprole.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPromptMessage
    \inmodule QtMcpCommon
    \brief Describes a message returned as part of a prompt.
    
    This is similar to `SamplingMessage`, but also supports the embedding of
    resources from the MCP server.
*/
class Q_MCPCOMMON_EXPORT QMcpPromptMessage : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QMcpPromptMessageContent content READ content WRITE setContent REQUIRED)

    Q_PROPERTY(QMcpRole::QMcpRole role READ role WRITE setRole REQUIRED)

public:
    QMcpPromptMessage() : QMcpGadget(new Private) {}

    QMcpPromptMessageContent content() const {
        return d<Private>()->content;
    }

    void setContent(const QMcpPromptMessageContent& content) {
        if (this->content() == content) return;
        d<Private>()->content = content;
    }

    QMcpRole::QMcpRole role() const {
        return d<Private>()->role;
    }

    void setRole(const QMcpRole::QMcpRole& role) {
        if (this->role() == role) return;
        d<Private>()->role = role;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpPromptMessageContent content;
        QMcpRole::QMcpRole role = QMcpRole::user;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPPROMPTMESSAGE_H
