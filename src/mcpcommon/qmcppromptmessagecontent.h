// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROMPTMESSAGECONTENT_H
#define QMCPPROMPTMESSAGECONTENT_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qmcpimagecontent.h>
#include <QtMcpCommon/qmcpaudiocontent.h>
#include <QtMcpCommon/qmcpembeddedresource.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpPromptMessageContent : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpTextContent textContent READ textContent WRITE setTextContent)
    Q_PROPERTY(QMcpImageContent imageContent READ imageContent WRITE setImageContent)
    Q_PROPERTY(QMcpAudioContent audioContent READ audioContent WRITE setAudioContent)
    Q_PROPERTY(QMcpEmbeddedResource embeddedResource READ embeddedResource WRITE setEmbeddedResource)
public:
    QMcpPromptMessageContent() : QMcpAnyOf(new Private) {}
    QMcpPromptMessageContent(const QMcpTextContent &textContent)
        : QMcpAnyOf(new Private)
    {
        setRefType("textContent"_ba);
        d<Private>()->textContent = textContent;
    }
    QMcpPromptMessageContent(const QMcpImageContent &imageContent)
        : QMcpAnyOf(new Private)
    {
        setRefType("imageContent"_ba);
        d<Private>()->imageContent = imageContent;
    }
    QMcpPromptMessageContent(const QMcpAudioContent &audioContent)
        : QMcpAnyOf(new Private)
    {
        setRefType("audioContent"_ba);
        d<Private>()->audioContent = audioContent;
    }
    QMcpPromptMessageContent(const QMcpEmbeddedResource &embeddedResource)
        : QMcpAnyOf(new Private)
    {
        setRefType("embeddedResource"_ba);
        d<Private>()->embeddedResource = embeddedResource;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QMcpTextContent textContent() const {
        return d<Private>()->textContent;
    }

    void setTextContent(const QMcpTextContent &textContent) {
        if (this->textContent() == textContent) return;
        setRefType("textContent"_ba);
        d<Private>()->textContent = textContent;
    }

    QMcpImageContent imageContent() const {
        return d<Private>()->imageContent;
    }

    void setImageContent(const QMcpImageContent &imageContent) {
        if (this->imageContent() == imageContent) return;
        setRefType("imageContent"_ba);
        d<Private>()->imageContent = imageContent;
    }

    QMcpAudioContent audioContent() const {
        return d<Private>()->audioContent;
    }

    void setAudioContent(const QMcpAudioContent &audioContent) {
        if (this->audioContent() == audioContent) return;
        setRefType("audioContent"_ba);
        d<Private>()->audioContent = audioContent;
    }

    QMcpEmbeddedResource embeddedResource() const {
        return d<Private>()->embeddedResource;
    }

    void setEmbeddedResource(const QMcpEmbeddedResource &embeddedResource) {
        if (this->embeddedResource() == embeddedResource) return;
        setRefType("embeddedResource"_ba);
        d<Private>()->embeddedResource = embeddedResource;
    }
private:
    struct Private : public QMcpAnyOf::Private {
        QMcpTextContent textContent;
        QMcpImageContent imageContent;
        QMcpAudioContent audioContent;
        QMcpEmbeddedResource embeddedResource;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPromptMessageContent)

QT_END_NAMESPACE

#endif // QMCPPROMPTMESSAGECONTENT_H
