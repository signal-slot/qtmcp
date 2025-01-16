// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPINITIALIZEDNOTIFICATIONPARAMS_H
#define QMCPINITIALIZEDNOTIFICATIONPARAMS_H

#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcpinitializednotificationparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpInitializedNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpInitializedNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpInitializedNotificationParams::_meta
        \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
    */
    Q_PROPERTY(QMcpInitializedNotificationParamsMeta _meta READ meta WRITE setMeta)

public:
    QMcpInitializedNotificationParams() : QMcpNotificationParams(new Private) {}

    QMcpInitializedNotificationParamsMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpInitializedNotificationParamsMeta &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QMcpInitializedNotificationParamsMeta _meta;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPINITIALIZEDNOTIFICATIONPARAMS_H
