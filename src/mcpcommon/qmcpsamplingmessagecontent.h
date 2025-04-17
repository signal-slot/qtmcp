// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSAMPLINGMESSAGECONTENT_H
#define QMCPSAMPLINGMESSAGECONTENT_H

#include <QtMcpCommon/qmcpmessagecontentbase.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpSamplingMessageContent : public QMcpMessageContentBase
{
    Q_GADGET
public:
    QMcpSamplingMessageContent() : QMcpMessageContentBase(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpMessageContentBase::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSamplingMessageContent)

QT_END_NAMESPACE

#endif // QMCPSAMPLINGMESSAGECONTENT_H
