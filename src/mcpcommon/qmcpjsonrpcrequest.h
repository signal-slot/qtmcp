// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCREQUEST_H
#define QMCPJSONRPCREQUEST_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpjsonrpcmessage.h>
#include <QtMcpCommon/qmcpjsonrpcrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCRequest
    \inmodule QtMcpCommon
    \brief A request that expects a response.
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCRequest : public QMcpJSONRPCMessageWithId
{
    Q_GADGET

    Q_PROPERTY(QString method READ method CONSTANT REQUIRED)
    Q_PROPERTY(QMcpJSONRPCRequestParams params READ params WRITE setParams)

public:
    QMcpJSONRPCRequest() : QMcpJSONRPCMessageWithId(new Private) {}
protected:
    QMcpJSONRPCRequest(Private *d) : QMcpJSONRPCMessageWithId(d) {}

public:
    virtual QString method() const = 0;

    QMcpJSONRPCRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpJSONRPCRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpJSONRPCMessageWithId::Private {
        QMcpJSONRPCRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPJSONRPCREQUEST_H
