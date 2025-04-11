// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCBATCHRESPONSE_H
#define QMCPJSONRPCBATCHRESPONSE_H

#include <QtCore/QList>
#include <QtMcpCommon/qmcpjsonrpcmessage.h>
#include <QtMcpCommon/qmcpjsonrpcresponse.h>
#include <QtMcpCommon/qmcpjsonrpcerror.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCBatchResponse
    \inmodule QtMcpCommon
    \brief A batch of JSON-RPC responses.
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCBatchResponse : public QMcpJSONRPCMessage
{
    Q_GADGET

    /*!
        \property QMcpJSONRPCBatchResponse::responses
        \brief The list of responses in this batch.
    */
    Q_PROPERTY(QList<QMcpJSONRPCResponse> responses READ responses WRITE setResponses REQUIRED)

public:
    QMcpJSONRPCBatchResponse() : QMcpJSONRPCMessage(new Private) {}

    QList<QMcpJSONRPCResponse> responses() const {
        return d<Private>()->responses;
    }

    void setResponses(const QList<QMcpJSONRPCResponse> &responses) {
        if (this->responses() == responses) return;
        d<Private>()->responses = responses;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpJSONRPCMessage::Private {
        QList<QMcpJSONRPCResponse> responses;

        Private *clone() const override { return new Private(*this); }
    };
};

/*! \class QMcpJSONRPCBatchErrorResponse
    \inmodule QtMcpCommon
    \brief A batch of JSON-RPC error responses.
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCBatchErrorResponse : public QMcpJSONRPCMessage
{
    Q_GADGET

    /*!
        \property QMcpJSONRPCBatchErrorResponse::errors
        \brief The list of error responses in this batch.
    */
    Q_PROPERTY(QList<QMcpJSONRPCError> errors READ errors WRITE setErrors REQUIRED)

public:
    QMcpJSONRPCBatchErrorResponse() : QMcpJSONRPCMessage(new Private) {}

    QList<QMcpJSONRPCError> errors() const {
        return d<Private>()->errors;
    }

    void setErrors(const QList<QMcpJSONRPCError> &errors) {
        if (this->errors() == errors) return;
        d<Private>()->errors = errors;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpJSONRPCMessage::Private {
        QList<QMcpJSONRPCError> errors;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpJSONRPCBatchResponse)
Q_DECLARE_SHARED(QMcpJSONRPCBatchErrorResponse)

QT_END_NAMESPACE

#endif // QMCPJSONRPCBATCHRESPONSE_H