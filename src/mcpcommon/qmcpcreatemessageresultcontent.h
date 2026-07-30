// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCREATEMESSAGERESULTCONTENT_H
#define QMCPCREATEMESSAGERESULTCONTENT_H

#include <QtMcpCommon/qmcpsamplingmessagecontentbase.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpCreateMessageResultContent : public QMcpSamplingMessageContentBase
{
    Q_GADGET
public:
    QMcpCreateMessageResultContent() : QMcpSamplingMessageContentBase(new Private) {}
    QMcpCreateMessageResultContent(const QMcpTextContent &textContent)
        : QMcpSamplingMessageContentBase(new Private) { setTextContent(textContent); }
    QMcpCreateMessageResultContent(const QMcpImageContent &imageContent)
        : QMcpSamplingMessageContentBase(new Private) { setImageContent(imageContent); }
    QMcpCreateMessageResultContent(const QMcpAudioContent &audioContent)
        : QMcpSamplingMessageContentBase(new Private) { setAudioContent(audioContent); }
    /*! \since MCP 2025-11-25 */
    QMcpCreateMessageResultContent(const QMcpToolUseContent &toolUse)
        : QMcpSamplingMessageContentBase(new Private) { setToolUse(toolUse); }
    /*! \since MCP 2025-11-25 */
    QMcpCreateMessageResultContent(const QMcpToolResultContent &toolResult)
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

Q_DECLARE_SHARED(QMcpCreateMessageResultContent)

QT_END_NAMESPACE

#endif // QMCPCREATEMESSAGERESULTCONTENT_H
