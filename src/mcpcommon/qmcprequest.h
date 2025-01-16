// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPREQUEST_H
#define QMCPREQUEST_H

#include <QtMcpCommon/qmcpjsonrpcrequest.h>
#include <QtMcpCommon/qmcprequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpRequest
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpRequest : public QMcpJSONRPCRequest
{
    Q_GADGET

#if 0
    Q_PROPERTY(QMcpRequestParams params READ params WRITE setParams)
#endif

public:
    QMcpRequest() : QMcpRequest(new Private) {}
protected:
    QMcpRequest(Private *d) : QMcpJSONRPCRequest(d) {}
public:

#if 0
    QMcpRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }
#endif

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpJSONRPCRequest::Private {
#if 0
        QMcpRequestParams params;
#endif

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPREQUEST_H
