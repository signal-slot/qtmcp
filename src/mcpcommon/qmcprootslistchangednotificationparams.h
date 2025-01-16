// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPROOTSLISTCHANGEDNOTIFICATIONPARAMS_H
#define QMCPROOTSLISTCHANGEDNOTIFICATIONPARAMS_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcprootslistchangednotificationparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpRootsListChangedNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpRootsListChangedNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpRootsListChangedNotificationParams::_meta
        \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
    */
    Q_PROPERTY(QMcpRootsListChangedNotificationParamsMeta _meta READ meta WRITE setMeta)

public:
    QMcpRootsListChangedNotificationParams() : QMcpNotificationParams(new Private) {}

    QMcpRootsListChangedNotificationParamsMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpRootsListChangedNotificationParamsMeta &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QMcpRootsListChangedNotificationParamsMeta _meta;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPROOTSLISTCHANGEDNOTIFICATIONPARAMS_H
