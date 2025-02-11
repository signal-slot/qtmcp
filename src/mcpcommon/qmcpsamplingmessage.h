// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSAMPLINGMESSAGE_H
#define QMCPSAMPLINGMESSAGE_H

#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcprole.h>
#include <QtMcpCommon/qmcpsamplingmessagecontent.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSamplingMessage
    \inmodule QtMcpCommon
    \brief Describes a message issued to or received from an LLM API.
*/
class Q_MCPCOMMON_EXPORT QMcpSamplingMessage : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QMcpSamplingMessageContent content READ content WRITE setContent REQUIRED)

    Q_PROPERTY(QMcpRole::QMcpRole role READ role WRITE setRole REQUIRED)

public:
    QMcpSamplingMessage() : QMcpGadget(new Private) {}

    QMcpSamplingMessageContent content() const {
        return d<Private>()->content;
    }

    void setContent(const QMcpSamplingMessageContent& content) {
        if (this->content() == content) return;
        d<Private>()->content = content;
    }

    QMcpRole::QMcpRole role() const {
        return d<Private>()->role;
    }

    void setRole(const QMcpRole::QMcpRole& role) {
        if (this->role() == role) return;
        d<Private>()->role = role;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpSamplingMessageContent content;
        QMcpRole::QMcpRole role = QMcpRole::assistant;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSamplingMessage)

QT_END_NAMESPACE

#endif // QMCPSAMPLINGMESSAGE_H
