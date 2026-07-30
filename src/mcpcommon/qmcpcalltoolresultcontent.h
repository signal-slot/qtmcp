// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCALLTOOLRESULTCONTENT_H
#define QMCPCALLTOOLRESULTCONTENT_H

#include <QtMcpCommon/qmcpextendedmessagecontent.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpCallToolResultContent : public QMcpExtendedMessageContent
{
    Q_GADGET
public:
    QMcpCallToolResultContent() : QMcpExtendedMessageContent(new Private) {}
    QMcpCallToolResultContent(const QMcpTextContent &textContent)
        : QMcpExtendedMessageContent(new Private) { setTextContent(textContent); }
    QMcpCallToolResultContent(const QMcpImageContent &imageContent)
        : QMcpExtendedMessageContent(new Private) { setImageContent(imageContent); }
    QMcpCallToolResultContent(const QMcpAudioContent &audioContent)
        : QMcpExtendedMessageContent(new Private) { setAudioContent(audioContent); }
    QMcpCallToolResultContent(const QMcpEmbeddedResource &embeddedResource)
        : QMcpExtendedMessageContent(new Private) { setEmbeddedResource(embeddedResource); }
    QMcpCallToolResultContent(const QMcpResourceLink &resourceLink)
        : QMcpExtendedMessageContent(new Private) { setResourceLink(resourceLink); }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpExtendedMessageContent::Private {
        Private *clone() const override { return new Private(*this); }

        // findPropertyIndex() is inherited from QMcpExtendedMessageContent::Private.
    };
};

Q_DECLARE_SHARED(QMcpCallToolResultContent)

QT_END_NAMESPACE

#endif // QMCPCALLTOOLRESULTCONTENT_H
