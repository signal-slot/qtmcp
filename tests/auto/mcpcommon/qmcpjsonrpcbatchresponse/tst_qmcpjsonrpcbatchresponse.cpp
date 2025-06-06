// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonValue>
#include <QtMcpCommon/qmcpjsonrpcbatchresponse.h>
#include <QtMcpCommon/qmcpjsonrpcresponse.h>
#include <QtMcpCommon/qmcpjsonrpcerror.h>
#include <QtMcpCommon/qmcpresult.h>
#include <QtTest/QTest>

class tst_QMcpJSONRPCBatchResponse : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void batchResponse();
    void batchErrorResponse();
    void copyBatchResponse();
    void copyBatchErrorResponse();
};

void tst_QMcpJSONRPCBatchResponse::defaultValues()
{
    // Test default values for QMcpJSONRPCBatchResponse
    QMcpJSONRPCBatchResponse batchResponse;
    QVERIFY(batchResponse.responses().isEmpty());
    QCOMPARE(batchResponse.jsonrpc(), "2.0"_L1);

    // Test default values for QMcpJSONRPCBatchErrorResponse
    QMcpJSONRPCBatchErrorResponse batchErrorResponse;
    QVERIFY(batchErrorResponse.errors().isEmpty());
    QCOMPARE(batchErrorResponse.jsonrpc(), "2.0"_L1);
}

void tst_QMcpJSONRPCBatchResponse::batchResponse()
{
    // Create a batch of JSON-RPC responses
    QMcpJSONRPCBatchResponse batchResponse;
    
    QList<QMcpJSONRPCResponse *> responses;
    
    // Create response objects on the heap to avoid dangling pointers
    // Create first response
    QMcpJSONRPCResponse *response1 = new QMcpJSONRPCResponse();
    response1->setId(1);
    QMcpResult result1;
    QJsonObject resultData1{{"key1", "value1"}};
    result1.setAdditionalProperties(resultData1);
    response1->setResult(result1);
    responses.append(response1);
    
    // Create second response
    QMcpJSONRPCResponse *response2 = new QMcpJSONRPCResponse();
    response2->setId("resp-2");
    QMcpResult result2;
    QJsonObject resultData2{{"key2", 42}};
    result2.setAdditionalProperties(resultData2);
    response2->setResult(result2);
    responses.append(response2);
    
    batchResponse.setResponses(responses);
    
    responses = batchResponse.responses();

    // Verify the batch response
    QCOMPARE(responses.size(), 2);
    auto response = responses.at(0);
    QCOMPARE(response->id().toInt(), 1);
    QCOMPARE(response->result().additionalProperties().value("key1").toString(), "value1"_L1);
    response = responses.at(1);
    QCOMPARE(response->id().toString(), "resp-2"_L1);
    QCOMPARE(response->result().additionalProperties().value("key2").toInt(), 42);
    
    // Test JSON conversion
    QJsonObject jsonObj = batchResponse.toJsonObject();
    QCOMPARE(jsonObj.value("jsonrpc").toString(), "2.0"_L1);
    QVERIFY(jsonObj.contains("responses"));
    QVERIFY(jsonObj.value("responses").isArray());
    QJsonArray responsesArray = jsonObj.value("responses").toArray();
    QCOMPARE(responsesArray.size(), 2);
    
    // Verify first response in JSON
    QJsonObject resp1Json = responsesArray.at(0).toObject();
    QCOMPARE(resp1Json.value("id").toInt(), 1);
    QCOMPARE(resp1Json.value("result").toObject().value("additionalProperties").toObject().value("key1").toString(), "value1"_L1);
    
    // Verify second response in JSON
    QJsonObject resp2Json = responsesArray.at(1).toObject();
    QCOMPARE(resp2Json.value("id").toString(), "resp-2"_L1);
    QCOMPARE(resp2Json.value("result").toObject().value("additionalProperties").toObject().value("key2").toInt(), 42);
    
    // Test JSON parsing
    QMcpJSONRPCBatchResponse parsedResponse;
    QVERIFY(parsedResponse.fromJsonObject(jsonObj));
    responses = parsedResponse.responses();
    QCOMPARE(responses.size(), 2);
    response = responses.at(0);
    QCOMPARE(response->id().toInt(), 1);
    QCOMPARE(response->result().additionalProperties().value("key1").toString(), "value1"_L1);
    response = responses.at(1);
    QCOMPARE(response->id().toString(), "resp-2"_L1);
    QCOMPARE(response->result().additionalProperties().value("key2").toInt(), 42);
    // Clean up allocated objects
    qDeleteAll(responses);
}

void tst_QMcpJSONRPCBatchResponse::batchErrorResponse()
{
    // Create a batch of JSON-RPC error responses
    QMcpJSONRPCBatchErrorResponse batchErrorResponse;
    
    QList<QMcpJSONRPCError> errors;
    
    // Create first error
    QMcpJSONRPCError error1;
    error1.setId(1);
    QMcpJSONRPCErrorError errorDetails1;
    errorDetails1.setCode(-32600);
    errorDetails1.setMessage("Invalid Request");
    QJsonObject errorData1{{"details", "Error details 1"}};
    errorDetails1.setData(errorData1);
    error1.setError(errorDetails1);
    errors.append(error1);
    
    // Create second error
    QMcpJSONRPCError error2;
    error2.setId("err-2");
    QMcpJSONRPCErrorError errorDetails2;
    errorDetails2.setCode(-32601);
    errorDetails2.setMessage("Method not found");
    QJsonObject errorData2{{"details", "Error details 2"}};
    errorDetails2.setData(errorData2);
    error2.setError(errorDetails2);
    errors.append(error2);
    
    batchErrorResponse.setErrors(errors);
    
    // Verify the batch error response
    QCOMPARE(batchErrorResponse.errors().size(), 2);
    QCOMPARE(batchErrorResponse.errors().at(0).id().toInt(), 1);
    QCOMPARE(batchErrorResponse.errors().at(0).error().code(), -32600);
    QCOMPARE(batchErrorResponse.errors().at(0).error().message(), "Invalid Request"_L1);
    QCOMPARE(batchErrorResponse.errors().at(0).error().data().toObject().value("details").toString(), "Error details 1"_L1);
    QCOMPARE(batchErrorResponse.errors().at(1).id().toString(), "err-2"_L1);
    QCOMPARE(batchErrorResponse.errors().at(1).error().code(), -32601);
    QCOMPARE(batchErrorResponse.errors().at(1).error().message(), "Method not found"_L1);
    QCOMPARE(batchErrorResponse.errors().at(1).error().data().toObject().value("details").toString(), "Error details 2"_L1);
    
    // Test JSON conversion
    QJsonObject jsonObj = batchErrorResponse.toJsonObject();
    QCOMPARE(jsonObj.value("jsonrpc").toString(), "2.0"_L1);
    QVERIFY(jsonObj.contains("errors"));
    QVERIFY(jsonObj.value("errors").isArray());
    QJsonArray errorsArray = jsonObj.value("errors").toArray();
    QCOMPARE(errorsArray.size(), 2);
    
    // Verify first error in JSON
    QJsonObject err1Json = errorsArray.at(0).toObject();
    QCOMPARE(err1Json.value("id").toInt(), 1);
    QCOMPARE(err1Json.value("error").toObject().value("code").toInt(), -32600);
    QCOMPARE(err1Json.value("error").toObject().value("message").toString(), "Invalid Request"_L1);
    QCOMPARE(err1Json.value("error").toObject().value("data").toObject().value("details").toString(), "Error details 1"_L1);
    
    // Verify second error in JSON
    QJsonObject err2Json = errorsArray.at(1).toObject();
    QCOMPARE(err2Json.value("id").toString(), "err-2"_L1);
    QCOMPARE(err2Json.value("error").toObject().value("code").toInt(), -32601);
    QCOMPARE(err2Json.value("error").toObject().value("message").toString(), "Method not found"_L1);
    QCOMPARE(err2Json.value("error").toObject().value("data").toObject().value("details").toString(), "Error details 2"_L1);
    
    // Test JSON parsing
    QMcpJSONRPCBatchErrorResponse parsedErrorResponse;
    QVERIFY(parsedErrorResponse.fromJsonObject(jsonObj));
    QCOMPARE(parsedErrorResponse.errors().size(), 2);
    QCOMPARE(parsedErrorResponse.errors().at(0).id().toInt(), 1);
    QCOMPARE(parsedErrorResponse.errors().at(0).error().code(), -32600);
    QCOMPARE(parsedErrorResponse.errors().at(0).error().message(), "Invalid Request"_L1);
    QCOMPARE(parsedErrorResponse.errors().at(1).id().toString(), "err-2"_L1);
    QCOMPARE(parsedErrorResponse.errors().at(1).error().code(), -32601);
    QCOMPARE(parsedErrorResponse.errors().at(1).error().message(), "Method not found"_L1);
}

void tst_QMcpJSONRPCBatchResponse::copyBatchResponse()
{
    // Create a batch response
    QMcpJSONRPCBatchResponse batchResponse;
    
    QList<QMcpJSONRPCResponse *> responses;
    
    // Create response objects on the heap to avoid dangling pointers
    QMcpJSONRPCResponse *response1 = new QMcpJSONRPCResponse();
    response1->setId(1);
    QMcpResult result1;
    QJsonObject resultData1{{"key1", "value1"}};
    result1.setAdditionalProperties(resultData1);
    response1->setResult(result1);
    responses.append(response1);
    
    QMcpJSONRPCResponse *response2 = new QMcpJSONRPCResponse();
    response2->setId("resp-2");
    QMcpResult result2;
    QJsonObject resultData2{{"key2", 42}};
    result2.setAdditionalProperties(resultData2);
    response2->setResult(result2);
    responses.append(response2);
    
    batchResponse.setResponses(responses);
    
    // Test copy constructor
    QMcpJSONRPCBatchResponse copiedResponse(batchResponse);
    QCOMPARE(copiedResponse.responses().size(), 2);
    QCOMPARE(copiedResponse.responses().at(0)->id().toInt(), 1);
    QCOMPARE(copiedResponse.responses().at(0)->result().additionalProperties().value("key1").toString(), "value1"_L1);
    QCOMPARE(copiedResponse.responses().at(1)->id().toString(), "resp-2"_L1);
    QCOMPARE(copiedResponse.responses().at(1)->result().additionalProperties().value("key2").toInt(), 42);
    
    // Test assignment operator
    QMcpJSONRPCBatchResponse assignedResponse;
    assignedResponse = batchResponse;
    QCOMPARE(assignedResponse.responses().size(), 2);
    QCOMPARE(assignedResponse.responses().at(0)->id().toInt(), 1);
    QCOMPARE(assignedResponse.responses().at(0)->result().additionalProperties().value("key1").toString(), "value1"_L1);
    QCOMPARE(assignedResponse.responses().at(1)->id().toString(), "resp-2"_L1);
    QCOMPARE(assignedResponse.responses().at(1)->result().additionalProperties().value("key2").toInt(), 42);
    // Clean up allocated objects
    qDeleteAll(responses);
}

void tst_QMcpJSONRPCBatchResponse::copyBatchErrorResponse()
{
    // Create a batch error response
    QMcpJSONRPCBatchErrorResponse batchErrorResponse;
    
    QList<QMcpJSONRPCError> errors;
    
    QMcpJSONRPCError error1;
    error1.setId(1);
    QMcpJSONRPCErrorError errorDetails1;
    errorDetails1.setCode(-32600);
    errorDetails1.setMessage("Invalid Request");
    QJsonObject errorData1{{"details", "Error details 1"}};
    errorDetails1.setData(errorData1);
    error1.setError(errorDetails1);
    errors.append(error1);
    
    QMcpJSONRPCError error2;
    error2.setId("err-2");
    QMcpJSONRPCErrorError errorDetails2;
    errorDetails2.setCode(-32601);
    errorDetails2.setMessage("Method not found");
    QJsonObject errorData2{{"details", "Error details 2"}};
    errorDetails2.setData(errorData2);
    error2.setError(errorDetails2);
    errors.append(error2);
    
    batchErrorResponse.setErrors(errors);
    
    // Test copy constructor
    QMcpJSONRPCBatchErrorResponse copiedErrorResponse(batchErrorResponse);
    QCOMPARE(copiedErrorResponse.errors().size(), 2);
    QCOMPARE(copiedErrorResponse.errors().at(0).id().toInt(), 1);
    QCOMPARE(copiedErrorResponse.errors().at(0).error().code(), -32600);
    QCOMPARE(copiedErrorResponse.errors().at(0).error().message(), "Invalid Request"_L1);
    QCOMPARE(copiedErrorResponse.errors().at(1).id().toString(), "err-2"_L1);
    QCOMPARE(copiedErrorResponse.errors().at(1).error().code(), -32601);
    QCOMPARE(copiedErrorResponse.errors().at(1).error().message(), "Method not found"_L1);
    
    // Test assignment operator
    QMcpJSONRPCBatchErrorResponse assignedErrorResponse;
    assignedErrorResponse = batchErrorResponse;
    QCOMPARE(assignedErrorResponse.errors().size(), 2);
    QCOMPARE(assignedErrorResponse.errors().at(0).id().toInt(), 1);
    QCOMPARE(assignedErrorResponse.errors().at(0).error().code(), -32600);
    QCOMPARE(assignedErrorResponse.errors().at(0).error().message(), "Invalid Request"_L1);
    QCOMPARE(assignedErrorResponse.errors().at(1).id().toString(), "err-2"_L1);
    QCOMPARE(assignedErrorResponse.errors().at(1).error().code(), -32601);
    QCOMPARE(assignedErrorResponse.errors().at(1).error().message(), "Method not found"_L1);
}

QTEST_MAIN(tst_QMcpJSONRPCBatchResponse)
#include "tst_qmcpjsonrpcbatchresponse.moc"
