// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPROOTSLISTCHANGEDNOTIFICATION_H
#define QMCPROOTSLISTCHANGEDNOTIFICATION_H

#include <QtMcpCommon/qmcpnotification.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpRootsListChangedNotification
    \inmodule QtMcpCommon
    \brief A notification from the client to the server, informing it that the list of roots has changed.
    This notification should be sent whenever the client adds, removes, or modifies any root.
    The server should then request an updated list of roots using the ListRootsRequest.
*/
class Q_MCPCOMMON_EXPORT QMcpRootsListChangedNotification : public QMcpNotification
{
    Q_GADGET

public:
    QMcpRootsListChangedNotification() : QMcpNotification(new Private) {}

    QString method() const final {
        return "notifications/roots/list_changed"_L1;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotification::Private {

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpRootsListChangedNotification)

QT_END_NAMESPACE

#endif // QMCPROOTSLISTCHANGEDNOTIFICATION_H
