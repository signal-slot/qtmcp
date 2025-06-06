// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCELISTCHANGEDNOTIFICATION_H
#define QMCPRESOURCELISTCHANGEDNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceListChangedNotification
    \inmodule QtMcpCommon
    \brief An optional notification from the server to the client, informing it that the list of resources it can read from has changed. This may be issued by servers without any previous subscription from the client.
*/
class Q_MCPCOMMON_EXPORT QMcpResourceListChangedNotification : public QMcpNotification
{
    Q_GADGET

public:
    QMcpResourceListChangedNotification() : QMcpNotification(new Private) {}


    QString method() const final {
        return "notifications/resources/list_changed"_L1;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpResourceListChangedNotification)

QT_END_NAMESPACE

#endif // QMCPRESOURCELISTCHANGEDNOTIFICATION_H
