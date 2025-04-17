// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCREATEMESSAGERESULTCONTENT_H
#define QMCPCREATEMESSAGERESULTCONTENT_H

#include <QtMcpCommon/qmcpmessagecontentbase.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpCreateMessageResultContent : public QMcpMessageContentBase
{
    Q_GADGET
public:
    QMcpCreateMessageResultContent() : QMcpMessageContentBase(new Private) {}
    QMcpCreateMessageResultContent(const QMcpTextContent &textContent)
        : QMcpMessageContentBase(new Private) { setTextContent(textContent); }
    QMcpCreateMessageResultContent(const QMcpImageContent &imageContent)
        : QMcpMessageContentBase(new Private) { setImageContent(imageContent); }
    QMcpCreateMessageResultContent(const QMcpAudioContent &audioContent)
        : QMcpMessageContentBase(new Private) { setAudioContent(audioContent); }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpMessageContentBase::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCreateMessageResultContent)

QT_END_NAMESPACE

#endif // QMCPCREATEMESSAGERESULTCONTENT_H
