// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROMPTMESSAGECONTENT_H
#define QMCPPROMPTMESSAGECONTENT_H

#include <QtMcpCommon/qmcpextendedmessagecontent.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpPromptMessageContent : public QMcpExtendedMessageContent
{
    Q_GADGET
public:
    QMcpPromptMessageContent() : QMcpExtendedMessageContent(new Private) {}
    QMcpPromptMessageContent(const QMcpTextContent &textContent)
        : QMcpExtendedMessageContent(new Private)
    {
        setTextContent(textContent);
    }
    QMcpPromptMessageContent(const QMcpImageContent &imageContent)
        : QMcpExtendedMessageContent(new Private)
    {
        setImageContent(imageContent);
    }
    QMcpPromptMessageContent(const QMcpAudioContent &audioContent)
        : QMcpExtendedMessageContent(new Private)
    {
        setAudioContent(audioContent);
    }
    QMcpPromptMessageContent(const QMcpEmbeddedResource &embeddedResource)
        : QMcpExtendedMessageContent(new Private)
    {
        setEmbeddedResource(embeddedResource);
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpExtendedMessageContent::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPromptMessageContent)

QT_END_NAMESPACE

#endif // QMCPPROMPTMESSAGECONTENT_H
