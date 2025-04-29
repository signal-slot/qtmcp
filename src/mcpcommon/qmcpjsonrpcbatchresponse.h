// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCBATCHRESPONSE_H
#define QMCPJSONRPCBATCHRESPONSE_H

#include <QtCore/QList>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
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
    Q_PROPERTY(QList<QMcpJSONRPCResponse *> responses READ responses WRITE setResponses REQUIRED)

public:
    QMcpJSONRPCBatchResponse() : QMcpJSONRPCMessage(new Private) {}

    // Override fromJsonObject to properly handle response list parsing
    bool fromJsonObject(const QJsonObject &json, const QString &protocolVersion = "2025-03-26"_L1) override {
        // First, call the base class implementation
        if (!QMcpJSONRPCMessage::fromJsonObject(json, protocolVersion))
            return false;
            
        // Handle responses array manually
        if (json.contains("responses"_L1) && json["responses"_L1].isArray()) {
            QJsonArray responsesArray = json["responses"_L1].toArray();
            QList<QMcpJSONRPCResponse*> responseList;
            
            // Process each response in the array
            for (const QJsonValue &value : responsesArray) {
                if (!value.isObject())
                    continue;
                    
                // Create a new response object
                QMcpJSONRPCResponse* response = new QMcpJSONRPCResponse();
                
                // Populate it from the JSON object
                if (!response->fromJsonObject(value.toObject(), protocolVersion)) {
                    delete response;
                    qDeleteAll(responseList);
                    responseList.clear();
                    return false;
                }
                
                // Add it to our list
                responseList.append(response);
            }
            
            // Update our responses property with the parsed list
            setResponses(responseList);
            return true;
        }
        
        return false;
    }
    
    QList<QMcpJSONRPCResponse *> responses() const {
        return d<Private>()->responses;
    }

    void setResponses(const QList<QMcpJSONRPCResponse *> &responses) {
        if (this->responses() == responses) return;
        d<Private>()->responses = responses;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpJSONRPCMessage::Private {
        QList<QMcpJSONRPCResponse *> responses;

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
