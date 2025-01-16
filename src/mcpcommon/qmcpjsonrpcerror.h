// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCERROR_H
#define QMCPJSONRPCERROR_H

#include <QtMcpCommon/qmcpjsonrpcmessage.h>
#include <QtMcpCommon/qmcpjsonrpcerrorerror.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCError
    \inmodule QtMcpCommon
    \brief A response to a request that indicates an error occurred.
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCError : public QMcpJSONRPCMessageWithId
{
    Q_GADGET

    Q_PROPERTY(QMcpJSONRPCErrorError error READ error WRITE setError REQUIRED)

public:
    QMcpJSONRPCError() : QMcpJSONRPCMessageWithId(new Private) {}

    QMcpJSONRPCErrorError error() const {
        return d<Private>()->error;
    }

    void setError(const QMcpJSONRPCErrorError &error) {
        if (this->error() == error) return;
        d<Private>()->error = error;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpJSONRPCMessageWithId::Private {
        QMcpJSONRPCErrorError error;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPJSONRPCERROR_H
