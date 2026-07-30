// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSAMPLINGMESSAGECONTENT_H
#define QMCPSAMPLINGMESSAGECONTENT_H

#include <QtMcpCommon/qmcpsamplingmessagecontentbase.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpSamplingMessageContent : public QMcpSamplingMessageContentBase
{
    Q_GADGET
public:
    QMcpSamplingMessageContent() : QMcpSamplingMessageContentBase(new Private) {}
    QMcpSamplingMessageContent(const QMcpTextContent &textContent)
        : QMcpSamplingMessageContentBase(new Private) { setTextContent(textContent); }
    QMcpSamplingMessageContent(const QMcpImageContent &imageContent)
        : QMcpSamplingMessageContentBase(new Private) { setImageContent(imageContent); }
    QMcpSamplingMessageContent(const QMcpAudioContent &audioContent)
        : QMcpSamplingMessageContentBase(new Private) { setAudioContent(audioContent); }
    /*! \since MCP 2025-11-25 */
    QMcpSamplingMessageContent(const QMcpToolUseContent &toolUse)
        : QMcpSamplingMessageContentBase(new Private) { setToolUse(toolUse); }
    /*! \since MCP 2025-11-25 */
    QMcpSamplingMessageContent(const QMcpToolResultContent &toolResult)
        : QMcpSamplingMessageContentBase(new Private) { setToolResult(toolResult); }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpSamplingMessageContentBase::Private {
        Private *clone() const override { return new Private(*this); }

        // findPropertyIndex() is inherited from QMcpSamplingMessageContentBase::Private.
    };
};

Q_DECLARE_SHARED(QMcpSamplingMessageContent)

QT_END_NAMESPACE

#endif // QMCPSAMPLINGMESSAGECONTENT_H
