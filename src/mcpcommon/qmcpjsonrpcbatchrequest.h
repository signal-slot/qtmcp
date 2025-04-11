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
    Q_PROPERTY(QList<QMcpJSONRPCRequest> requests READ requests WRITE setRequests REQUIRED)

public:
    QMcpJSONRPCBatchRequest() : QMcpJSONRPCMessage(new Private) {}

    QList<QMcpJSONRPCRequest> requests() const {
        return d<Private>()->requests;
    }

    void setRequests(const QList<QMcpJSONRPCRequest> &requests) {
        if (this->requests() == requests) return;
        d<Private>()->requests = requests;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpJSONRPCMessage::Private {
        QList<QMcpJSONRPCRequest> requests;

        Private *clone() const override { return new Private(*this); }
    };
};

/*! \class QMcpServerJSONRPCBatchRequest
    \inmodule QtMcpCommon
    \brief A batch of server JSON-RPC requests.
*/
class Q_MCPCOMMON_EXPORT QMcpServerJSONRPCBatchRequest : public QMcpJSONRPCMessage
{
    Q_GADGET

    /*!
        \property QMcpServerJSONRPCBatchRequest::requests
        \brief The list of server requests in this batch.
    */
    Q_PROPERTY(QList<QMcpServerRequest> requests READ requests WRITE setRequests REQUIRED)

public:
    QMcpServerJSONRPCBatchRequest() : QMcpJSONRPCMessage(new Private) {}

    QList<QMcpServerRequest> requests() const {
        return d<Private>()->requests;
    }

    void setRequests(const QList<QMcpServerRequest> &requests) {
        if (this->requests() == requests) return;
        d<Private>()->requests = requests;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpJSONRPCMessage::Private {
        QList<QMcpServerRequest> requests;

        Private *clone() const override { return new Private(*this); }
    };
};

/*! \class QMcpClientJSONRPCBatchRequest
    \inmodule QtMcpCommon
    \brief A batch of client JSON-RPC requests.
*/
class Q_MCPCOMMON_EXPORT QMcpClientJSONRPCBatchRequest : public QMcpJSONRPCMessage
{
    Q_GADGET

    /*!
        \property QMcpClientJSONRPCBatchRequest::requests
        \brief The list of client requests in this batch.
    */
    Q_PROPERTY(QList<QMcpClientRequest> requests READ requests WRITE setRequests REQUIRED)

public:
    QMcpClientJSONRPCBatchRequest() : QMcpJSONRPCMessage(new Private) {}

    QList<QMcpClientRequest> requests() const {
        return d<Private>()->requests;
    }

    void setRequests(const QList<QMcpClientRequest> &requests) {
        if (this->requests() == requests) return;
        d<Private>()->requests = requests;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpJSONRPCMessage::Private {
        QList<QMcpClientRequest> requests;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpJSONRPCBatchRequest)
Q_DECLARE_SHARED(QMcpServerJSONRPCBatchRequest)
Q_DECLARE_SHARED(QMcpClientJSONRPCBatchRequest)

QT_END_NAMESPACE

#endif // QMCPJSONRPCBATCHREQUEST_H