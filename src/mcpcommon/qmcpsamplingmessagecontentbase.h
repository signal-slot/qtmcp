// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSAMPLINGMESSAGECONTENTBASE_H
#define QMCPSAMPLINGMESSAGECONTENTBASE_H

#include <QtMcpCommon/qmcpmessagecontentbase.h>
#include <QtMcpCommon/qmcptoolresultcontent.h>
#include <QtMcpCommon/qmcptoolusecontent.h>

QT_BEGIN_NAMESPACE

/*!
    Adds the tool call variants of the schema's SamplingMessageContentBlock to
    the text, image and audio variants of QMcpMessageContentBase.

    Shared by QMcpSamplingMessageContent and QMcpCreateMessageResultContent:
    since MCP 2025-11-25 both SamplingMessage.content and
    CreateMessageResult.content accept tool_use and tool_result blocks, while
    the ContentBlock union used by tool and prompt results does not.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpSamplingMessageContentBase : public QMcpMessageContentBase
{
    Q_GADGET

    Q_PROPERTY(QMcpToolUseContent toolUse READ toolUse WRITE setToolUse)
    Q_PROPERTY(QMcpToolResultContent toolResult READ toolResult WRITE setToolResult)

public:
    QMcpToolUseContent toolUse() const {
        return d<Private>()->toolUse;
    }

    void setToolUse(const QMcpToolUseContent &toolUse) {
        if (this->toolUse() == toolUse) return;
        setRefType("toolUse"_ba);
        d<Private>()->toolUse = toolUse;
    }

    QMcpToolResultContent toolResult() const {
        return d<Private>()->toolResult;
    }

    void setToolResult(const QMcpToolResultContent &toolResult) {
        if (this->toolResult() == toolResult) return;
        setRefType("toolResult"_ba);
        d<Private>()->toolResult = toolResult;
    }

protected:
    // Protected constructor for base class
    QMcpSamplingMessageContentBase(Private *d) : QMcpMessageContentBase(d) {}

    struct Private : public QMcpMessageContentBase::Private {
        QMcpToolUseContent toolUse;
        QMcpToolResultContent toolResult;

        int findPropertyIndex(const QJsonObject &object) const override {
            // The spec discriminates the content blocks by their "type", so use
            // it instead of relying on the generic property matching of
            // QMcpAnyOf.
            const auto type = object.value("type"_L1).toString();
            const auto mo = QMcpSamplingMessageContentBase::staticMetaObject;
            if (type == "tool_use"_L1)
                return mo.indexOfProperty("toolUse");
            if (type == "tool_result"_L1)
                return mo.indexOfProperty("toolResult");
            return QMcpMessageContentBase::Private::findPropertyIndex(object);
        }
    };
};

QT_END_NAMESPACE

#endif // QMCPSAMPLINGMESSAGECONTENTBASE_H
