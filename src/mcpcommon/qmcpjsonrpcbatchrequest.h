// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCBATCHREQUEST_H
#define QMCPJSONRPCBATCHREQUEST_H

#include <QtCore/QList>
#include <QtMcpCommon/qmcpjsonrpcmessage.h>
#include <QtMcpCommon/qmcpjsonrpcrequest.h>
#include <QtMcpCommon/qmcpserverrequest.h>
#include <QtMcpCommon/qmcpclientrequest.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCBatchRequest
    \inmodule QtMcpCommon
    \brief A batch of JSON-RPC requests.
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCBatchRequest : public QMcpJSONRPCMessage
{
    Q_GADGET

    /*!
        \property QMcpJSONRPCBatchRequest::requests
        \brief The list of requests in this batch.
    */
    Q_PROPERTY(QList<QMcpJSONRPCRequest *> requests READ requests WRITE setRequests REQUIRED)

public:
    QMcpJSONRPCBatchRequest() : QMcpJSONRPCMessage(new Private) {}

    QList<QMcpJSONRPCRequest *> requests() const {
        return d<Private>()->requests;
    }

    void setRequests(const QList<QMcpJSONRPCRequest *> &requests) {
        if (this->requests() == requests) return;
        d<Private>()->requests = requests;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpJSONRPCMessage::Private {
        QList<QMcpJSONRPCRequest *> requests;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpJSONRPCBatchRequest)

QT_END_NAMESPACE

#endif // QMCPJSONRPCBATCHREQUEST_H
