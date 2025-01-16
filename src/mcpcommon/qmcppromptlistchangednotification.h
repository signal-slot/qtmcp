// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROMPTLISTCHANGEDNOTIFICATION_H
#define QMCPPROMPTLISTCHANGEDNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcppromptlistchangednotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPromptListChangedNotification
    \inmodule QtMcpCommon
    \brief An optional notification from the server to the client, informing it that the list of prompts it offers has changed. This may be issued by servers without any previous subscription from the client.
*/
class Q_MCPCOMMON_EXPORT QMcpPromptListChangedNotification : public QMcpNotification
{
    Q_GADGET

    Q_PROPERTY(QMcpPromptListChangedNotificationParams params READ params WRITE setParams)

public:
    QMcpPromptListChangedNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/prompts/list_changed"_L1;
    }

    QMcpPromptListChangedNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpPromptListChangedNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        QMcpPromptListChangedNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPPROMPTLISTCHANGEDNOTIFICATION_H
