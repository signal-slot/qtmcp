// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCNOTIFICATIONPARAMS_H
#define QMCPJSONRPCNOTIFICATIONPARAMS_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpjsonrpcnotificationparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCNotificationParams : public QMcpGadget
{
    Q_GADGET

#if 0
    /*!
        \property QMcpJSONRPCNotificationParams::_meta
        \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
    */
    Q_PROPERTY(QMcpJSONRPCNotificationParamsMeta _meta READ _meta WRITE setMeta)

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)
#endif
public:
    QMcpJSONRPCNotificationParams() : QMcpGadget(new Private) {}
protected:
    QMcpJSONRPCNotificationParams(Private *d) : QMcpGadget(d) {}
public:

#if 0
    QMcpJSONRPCNotificationParamsMeta _meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpJSONRPCNotificationParamsMeta &meta) {
        if (_meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    QJsonObject additionalProperties() const {
        return d<Private>()->additionalProperties;
    }

    void setAdditionalProperties(const QJsonObject &properties) {
        if (additionalProperties() == properties) return;
        d<Private>()->additionalProperties = properties;
    }
#endif
    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpGadget::Private {
#if 0
        QMcpJSONRPCNotificationParamsMeta _meta;
        QJsonObject additionalProperties;
#endif
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpJSONRPCNotificationParams)

QT_END_NAMESPACE

#endif // QMCPJSONRPCNOTIFICATIONPARAMS_H
