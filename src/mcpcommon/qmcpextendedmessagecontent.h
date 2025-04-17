// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTENDEDMESSAGECONTENT_H
#define QMCPEXTENDEDMESSAGECONTENT_H

#include <QtMcpCommon/qmcpmessagecontentbase.h>
#include <QtMcpCommon/qmcpembeddedresource.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpExtendedMessageContent : public QMcpMessageContentBase
{
    Q_GADGET

    Q_PROPERTY(QMcpEmbeddedResource embeddedResource READ embeddedResource WRITE setEmbeddedResource)

public:
    QMcpEmbeddedResource embeddedResource() const {
        return d<Private>()->embeddedResource;
    }

    void setEmbeddedResource(const QMcpEmbeddedResource &embeddedResource) {
        if (this->embeddedResource() == embeddedResource) return;
        setRefType("embeddedResource"_ba);
        d<Private>()->embeddedResource = embeddedResource;
    }

protected:
    // Protected constructor for base class
    QMcpExtendedMessageContent(Private *d) : QMcpMessageContentBase(d) {}

    struct Private : public QMcpMessageContentBase::Private {
        QMcpEmbeddedResource embeddedResource;
    };
};

QT_END_NAMESPACE

#endif // QMCPEXTENDEDMESSAGECONTENT_H
