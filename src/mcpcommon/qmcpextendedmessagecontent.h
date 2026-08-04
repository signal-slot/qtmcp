// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTENDEDMESSAGECONTENT_H
#define QMCPEXTENDEDMESSAGECONTENT_H

#include <QtMcpCommon/qmcpmessagecontentbase.h>
#include <QtMcpCommon/qmcpembeddedresource.h>
#include <QtMcpCommon/qmcpresourcelink.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpExtendedMessageContent : public QMcpMessageContentBase
{
    Q_GADGET

    Q_PROPERTY(QMcpEmbeddedResource embeddedResource READ embeddedResource WRITE setEmbeddedResource)
    Q_PROPERTY(QMcpResourceLink resourceLink READ resourceLink WRITE setResourceLink)

public:
    QMcpEmbeddedResource embeddedResource() const {
        return d<Private>()->embeddedResource;
    }

    void setEmbeddedResource(const QMcpEmbeddedResource &embeddedResource) {
        if (this->embeddedResource() == embeddedResource) return;
        setRefType("embeddedResource"_ba);
        d<Private>()->embeddedResource = embeddedResource;
    }

    QMcpResourceLink resourceLink() const {
        return d<Private>()->resourceLink;
    }

    void setResourceLink(const QMcpResourceLink &resourceLink) {
        if (this->resourceLink() == resourceLink) return;
        setRefType("resourceLink"_ba);
        d<Private>()->resourceLink = resourceLink;
    }

protected:
    // Protected constructor for base class
    QMcpExtendedMessageContent(Private *d) : QMcpMessageContentBase(d) {}

    struct Private : public QMcpMessageContentBase::Private {
        QMcpEmbeddedResource embeddedResource;
        QMcpResourceLink resourceLink;

        int findPropertyIndex(const QJsonObject &object) const override {
            // A resource link may carry a mimeType, so it has to be detected by
            // its type discriminator before the mimeType based detection of the
            // base class kicks in.
            if (object.value("type"_L1).toString() == "resource_link"_L1) {
                const auto mo = QMcpExtendedMessageContent::staticMetaObject;
                return mo.indexOfProperty("resourceLink");
            }
            return QMcpMessageContentBase::Private::findPropertyIndex(object);
        }
    };
};

QT_END_NAMESPACE

#endif // QMCPEXTENDEDMESSAGECONTENT_H
