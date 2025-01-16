// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCEUPDATEDNOTIFICATIONPARAMS_H
#define QMCPRESOURCEUPDATEDNOTIFICATIONPARAMS_H

#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpnotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceUpdatedNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpResourceUpdatedNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpResourceUpdatedNotificationParams::uri
        \brief The URI of the resource that has been updated. This might be a sub-resource of the one that the client actually subscribed to.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

public:
    QMcpResourceUpdatedNotificationParams() : QMcpNotificationParams(new Private) {}

    QUrl uri() const {
        return d<Private>()->uri;
    }

    void setUri(const QUrl& uri) {
        if (this->uri() == uri) return;
        d<Private>()->uri = uri;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QUrl uri;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPRESOURCEUPDATEDNOTIFICATIONPARAMS_H
