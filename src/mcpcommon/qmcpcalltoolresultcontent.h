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

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpExtendedMessageContent::Private {
        Private *clone() const override { return new Private(*this); }

        int findPropertyIndex(const QJsonObject &object) const override {
            int ret = -1;
            if (object.contains("mimeType"_L1)) {
                // image and audio have same properties.
                const auto mimeType = object.value("mimeType"_L1).toString();
                const auto mo = QMcpCallToolResultContent::staticMetaObject;
                if (mimeType.startsWith("image/"_L1)) {
                    ret = mo.indexOfProperty("imageContent");
                } else if (mimeType.startsWith("audio/"_L1)) {
                    ret = mo.indexOfProperty("audioContent");
                }
            }
            return ret;
        }
    };
};

Q_DECLARE_SHARED(QMcpCallToolResultContent)

QT_END_NAMESPACE

#endif // QMCPCALLTOOLRESULTCONTENT_H
