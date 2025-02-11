// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCREQUESTPARAMS_H
#define QMCPJSONRPCREQUESTPARAMS_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpjsonrpcrequestparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCRequestParams : public QMcpGadget
{
    Q_GADGET

#if 0
    Q_PROPERTY(QMcpJSONRPCRequestParamsMeta _meta READ _meta WRITE setMeta)

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)
#endif

public:
    QMcpJSONRPCRequestParams() : QMcpGadget(new Private) {}

#if 0
    QMcpJSONRPCRequestParamsMeta _meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpJSONRPCRequestParamsMeta &meta) {
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

private:
    struct Private : public QMcpGadget::Private {
#if 0
        QMcpJSONRPCRequestParamsMeta _meta;
        QJsonObject additionalProperties;
#endif

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpJSONRPCRequestParams)

QT_END_NAMESPACE

#endif // QMCPJSONRPCREQUESTPARAMS_H
