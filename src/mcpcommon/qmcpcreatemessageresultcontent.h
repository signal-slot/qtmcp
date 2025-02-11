// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCREATEMESSAGERESULTCONTENT_H
#define QMCPCREATEMESSAGERESULTCONTENT_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qmcpimagecontent.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpCreateMessageResultContent : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpTextContent textContent READ textContent WRITE setTextContent)
    Q_PROPERTY(QMcpImageContent imageContent READ imageContent WRITE setImageContent)
public:
    QMcpCreateMessageResultContent() : QMcpAnyOf(new Private) {}

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

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpTextContent textContent;
        QMcpImageContent imageContent;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCreateMessageResultContent)

QT_END_NAMESPACE

#endif // QMCPCREATEMESSAGERESULTCONTENT_H
