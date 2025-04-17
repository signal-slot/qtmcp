// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPMESSAGECONTENTBASE_H
#define QMCPMESSAGECONTENTBASE_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qmcpimagecontent.h>
#include <QtMcpCommon/qmcpaudiocontent.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpMessageContentBase : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpTextContent textContent READ textContent WRITE setTextContent)
    Q_PROPERTY(QMcpImageContent imageContent READ imageContent WRITE setImageContent)
    Q_PROPERTY(QMcpAudioContent audioContent READ audioContent WRITE setAudioContent)

public:
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

protected:
    // Protected constructor for base class
    QMcpMessageContentBase(Private *d) : QMcpAnyOf(d) {}

    struct Private : public QMcpAnyOf::Private {
        QMcpTextContent textContent;
        QMcpImageContent imageContent;
        QMcpAudioContent audioContent;

        // Find property index based on JSON object content
        int findPropertyIndex(const QJsonObject &object) const override {
            int ret = -1;
            if (object.contains(QStringLiteral("mimeType"))) {
                // image and audio have same properties.
                const auto mimeType = object.value(QStringLiteral("mimeType")).toString();
                const auto mo = QMcpMessageContentBase::staticMetaObject;
                if (mimeType.startsWith(QStringLiteral("image/"))) {
                    ret = mo.indexOfProperty("imageContent");
                } else if (mimeType.startsWith(QStringLiteral("audio/"))) {
                    ret = mo.indexOfProperty("audioContent");
                }
            }
            return ret;
        }
    };
};

QT_END_NAMESPACE

#endif // QMCPMESSAGECONTENTBASE_H
