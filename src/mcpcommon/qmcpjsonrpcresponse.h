// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCRESPONSE_H
#define QMCPJSONRPCRESPONSE_H

#include <QtMcpCommon/qmcpjsonrpcmessage.h>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCResponse
    \inmodule QtMcpCommon
    \brief A successful (non-error) response to a request.
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCResponse : public QMcpJSONRPCMessageWithId
{
    Q_GADGET

    Q_PROPERTY(QMcpResult result READ result WRITE setResult REQUIRED)

public:
    QMcpJSONRPCResponse() : QMcpJSONRPCMessageWithId(new Private) {}

    QMcpResult result() const {
        return d<Private>()->result;
    }

    void setResult(const QMcpResult &result) {
        if (this->result() == result) return;
        d<Private>()->result = result;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpJSONRPCMessageWithId::Private {
        QMcpResult result;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPJSONRPCRESPONSE_H
