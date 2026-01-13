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

    Q_PROPERTY(QMcpJSONRPCRequestParamsMeta _meta READ meta WRITE setMeta)

public:
    QMcpJSONRPCRequestParams() : QMcpGadget(new Private) {}

    QMcpJSONRPCRequestParamsMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpJSONRPCRequestParamsMeta &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpJSONRPCRequestParamsMeta _meta;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpJSONRPCRequestParams)

QT_END_NAMESPACE

#endif // QMCPJSONRPCREQUESTPARAMS_H
