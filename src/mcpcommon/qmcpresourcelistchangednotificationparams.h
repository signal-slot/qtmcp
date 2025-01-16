// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCELISTCHANGEDNOTIFICATIONPARAMS_H
#define QMCPRESOURCELISTCHANGEDNOTIFICATIONPARAMS_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcpresourcelistchangednotificationparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceListChangedNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpResourceListChangedNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpResourceListChangedNotificationParams::_meta
        \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
    */
    Q_PROPERTY(QMcpResourceListChangedNotificationParamsMeta _meta READ meta WRITE setMeta)

public:
    QMcpResourceListChangedNotificationParams() : QMcpNotificationParams(new Private) {}

    QMcpResourceListChangedNotificationParamsMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpResourceListChangedNotificationParamsMeta &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QMcpResourceListChangedNotificationParamsMeta _meta;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPRESOURCELISTCHANGEDNOTIFICATIONPARAMS_H
