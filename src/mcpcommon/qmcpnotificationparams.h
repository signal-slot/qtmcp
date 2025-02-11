// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPNOTIFICATIONPARAMS_H
#define QMCPNOTIFICATIONPARAMS_H

#include <QtMcpCommon/qmcpjsonrpcnotificationparams.h>
#include <QtMcpCommon/qmcpnotificationparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpNotificationParams : public QMcpJSONRPCNotificationParams
{
    Q_GADGET

#if 0
    /*!
        \property QMcpNotificationParams::_meta
        \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
    */
    Q_PROPERTY(QMcpNotificationParamsMeta _meta READ _meta WRITE setMeta)
#endif

public:
    QMcpNotificationParams() : QMcpJSONRPCNotificationParams(new Private) {}
protected:
    QMcpNotificationParams(Private *d) : QMcpJSONRPCNotificationParams(d) {}
public:

#if 0
    QMcpNotificationParamsMeta _meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpNotificationParamsMeta &meta) {
        if (_meta() == meta) return;
        d<Private>()->_meta = meta;
    }
#endif

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpJSONRPCNotificationParams::Private {
#if 0
        QMcpNotificationParamsMeta _meta;
#endif

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpNotificationParams)

QT_END_NAMESPACE

#endif // QMCPNOTIFICATIONPARAMS_H
