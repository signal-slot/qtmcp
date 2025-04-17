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
    QCOMPARE(batchResponse.jsonrpc(), QString("2.0"));

    // Test default values for QMcpJSONRPCBatchErrorResponse
    QMcpJSONRPCBatchErrorResponse batchErrorResponse;
    QVERIFY(batchErrorResponse.errors().isEmpty());
    QCOMPARE(batchErrorResponse.jsonrpc(), QString("2.0"));
}

void tst_QMcpJSONRPCBatchResponse::batchResponse()
{
    // Create a batch of JSON-RPC responses
    QMcpJSONRPCBatchResponse batchResponse;
    
    QList<QMcpJSONRPCResponse> responses;
    
    // Create first response
    QMcpJSONRPCResponse response1;
    response1.setId(1);
    QMcpResult result1;
    QJsonObject resultData1{{"key1", "value1"}};
    result1.setAdditionalProperties(resultData1);
    response1.setResult(result1);
    responses.append(response1);
    
    // Create second response
    QMcpJSONRPCResponse response2;
    response2.setId("resp-2");
    QMcpResult result2;
    QJsonObject resultData2{{"key2", 42}};
    result2.setAdditionalProperties(resultData2);
    response2.setResult(result2);
    responses.append(response2);
    
    batchResponse.setResponses(responses);
    
    // Verify the batch response
    QCOMPARE(batchResponse.responses().size(), 2);
    QCOMPARE(batchResponse.responses()[0].id().toInt(), 1);
    QCOMPARE(batchResponse.responses()[0].result().additionalProperties()["key1"].toString(), QString("value1"));
    QCOMPARE(batchResponse.responses()[1].id().toString(), QString("resp-2"));
    QCOMPARE(batchResponse.responses()[1].result().additionalProperties()["key2"].toInt(), 42);
    
    // Test JSON conversion
    QJsonObject jsonObj = batchResponse.toJsonObject();
    QCOMPARE(jsonObj["jsonrpc"].toString(), QString("2.0"));
    QVERIFY(jsonObj.contains("responses"));
    QVERIFY(jsonObj["responses"].isArray());
    QJsonArray responsesArray = jsonObj["responses"].toArray();
    QCOMPARE(responsesArray.size(), 2);
    
    // Verify first response in JSON
    QJsonObject resp1Json = responsesArray[0].toObject();
    QCOMPARE(resp1Json["id"].toInt(), 1);
    QCOMPARE(resp1Json["result"].toObject()["key1"].toString(), QString("value1"));
    
    // Verify second response in JSON
    QJsonObject resp2Json = responsesArray[1].toObject();
    QCOMPARE(resp2Json["id"].toString(), QString("resp-2"));
    QCOMPARE(resp2Json["result"].toObject()["key2"].toInt(), 42);
    
    // Test JSON parsing
    QMcpJSONRPCBatchResponse parsedResponse;
    QVERIFY(parsedResponse.fromJsonObject(jsonObj));
    QCOMPARE(parsedResponse.responses().size(), 2);
    QCOMPARE(parsedResponse.responses()[0].id().toInt(), 1);
    QCOMPARE(parsedResponse.responses()[0].result().additionalProperties()["key1"].toString(), QString("value1"));
    QCOMPARE(parsedResponse.responses()[1].id().toString(), QString("resp-2"));
    QCOMPARE(parsedResponse.responses()[1].result().additionalProperties()["key2"].toInt(), 42);
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
    QCOMPARE(batchErrorResponse.errors()[0].id().toInt(), 1);
    QCOMPARE(batchErrorResponse.errors()[0].error().code(), -32600);
    QCOMPARE(batchErrorResponse.errors()[0].error().message(), QString("Invalid Request"));
    QCOMPARE(batchErrorResponse.errors()[0].error().data().toObject()["details"].toString(), QString("Error details 1"));
    QCOMPARE(batchErrorResponse.errors()[1].id().toString(), QString("err-2"));
    QCOMPARE(batchErrorResponse.errors()[1].error().code(), -32601);
    QCOMPARE(batchErrorResponse.errors()[1].error().message(), QString("Method not found"));
    QCOMPARE(batchErrorResponse.errors()[1].error().data().toObject()["details"].toString(), QString("Error details 2"));
    
    // Test JSON conversion
    QJsonObject jsonObj = batchErrorResponse.toJsonObject();
    QCOMPARE(jsonObj["jsonrpc"].toString(), QString("2.0"));
    QVERIFY(jsonObj.contains("errors"));
    QVERIFY(jsonObj["errors"].isArray());
    QJsonArray errorsArray = jsonObj["errors"].toArray();
    QCOMPARE(errorsArray.size(), 2);
    
    // Verify first error in JSON
    QJsonObject err1Json = errorsArray[0].toObject();
    QCOMPARE(err1Json["id"].toInt(), 1);
    QCOMPARE(err1Json["error"].toObject()["code"].toInt(), -32600);
    QCOMPARE(err1Json["error"].toObject()["message"].toString(), QString("Invalid Request"));
    QCOMPARE(err1Json["error"].toObject()["data"].toObject()["details"].toString(), QString("Error details 1"));
    
    // Verify second error in JSON
    QJsonObject err2Json = errorsArray[1].toObject();
    QCOMPARE(err2Json["id"].toString(), QString("err-2"));
    QCOMPARE(err2Json["error"].toObject()["code"].toInt(), -32601);
    QCOMPARE(err2Json["error"].toObject()["message"].toString(), QString("Method not found"));
    QCOMPARE(err2Json["error"].toObject()["data"].toObject()["details"].toString(), QString("Error details 2"));
    
    // Test JSON parsing
    QMcpJSONRPCBatchErrorResponse parsedErrorResponse;
    QVERIFY(parsedErrorResponse.fromJsonObject(jsonObj));
    QCOMPARE(parsedErrorResponse.errors().size(), 2);
    QCOMPARE(parsedErrorResponse.errors()[0].id().toInt(), 1);
    QCOMPARE(parsedErrorResponse.errors()[0].error().code(), -32600);
    QCOMPARE(parsedErrorResponse.errors()[0].error().message(), QString("Invalid Request"));
    QCOMPARE(parsedErrorResponse.errors()[1].id().toString(), QString("err-2"));
    QCOMPARE(parsedErrorResponse.errors()[1].error().code(), -32601);
    QCOMPARE(parsedErrorResponse.errors()[1].error().message(), QString("Method not found"));
}

void tst_QMcpJSONRPCBatchResponse::copyBatchResponse()
{
    // Create a batch response
    QMcpJSONRPCBatchResponse batchResponse;
    
    QList<QMcpJSONRPCResponse> responses;
    
    QMcpJSONRPCResponse response1;
    response1.setId(1);
    QMcpResult result1;
    QJsonObject resultData1{{"key1", "value1"}};
    result1.setAdditionalProperties(resultData1);
    response1.setResult(result1);
    responses.append(response1);
    
    QMcpJSONRPCResponse response2;
    response2.setId("resp-2");
    QMcpResult result2;
    QJsonObject resultData2{{"key2", 42}};
    result2.setAdditionalProperties(resultData2);
    response2.setResult(result2);
    responses.append(response2);
    
    batchResponse.setResponses(responses);
    
    // Test copy constructor
    QMcpJSONRPCBatchResponse copiedResponse(batchResponse);
    QCOMPARE(copiedResponse.responses().size(), 2);
    QCOMPARE(copiedResponse.responses()[0].id().toInt(), 1);
    QCOMPARE(copiedResponse.responses()[0].result().additionalProperties()["key1"].toString(), QString("value1"));
    QCOMPARE(copiedResponse.responses()[1].id().toString(), QString("resp-2"));
    QCOMPARE(copiedResponse.responses()[1].result().additionalProperties()["key2"].toInt(), 42);
    
    // Test assignment operator
    QMcpJSONRPCBatchResponse assignedResponse;
    assignedResponse = batchResponse;
    QCOMPARE(assignedResponse.responses().size(), 2);
    QCOMPARE(assignedResponse.responses()[0].id().toInt(), 1);
    QCOMPARE(assignedResponse.responses()[0].result().additionalProperties()["key1"].toString(), QString("value1"));
    QCOMPARE(assignedResponse.responses()[1].id().toString(), QString("resp-2"));
    QCOMPARE(assignedResponse.responses()[1].result().additionalProperties()["key2"].toInt(), 42);
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
    QCOMPARE(copiedErrorResponse.errors()[0].id().toInt(), 1);
    QCOMPARE(copiedErrorResponse.errors()[0].error().code(), -32600);
    QCOMPARE(copiedErrorResponse.errors()[0].error().message(), QString("Invalid Request"));
    QCOMPARE(copiedErrorResponse.errors()[1].id().toString(), QString("err-2"));
    QCOMPARE(copiedErrorResponse.errors()[1].error().code(), -32601);
    QCOMPARE(copiedErrorResponse.errors()[1].error().message(), QString("Method not found"));
    
    // Test assignment operator
    QMcpJSONRPCBatchErrorResponse assignedErrorResponse;
    assignedErrorResponse = batchErrorResponse;
    QCOMPARE(assignedErrorResponse.errors().size(), 2);
    QCOMPARE(assignedErrorResponse.errors()[0].id().toInt(), 1);
    QCOMPARE(assignedErrorResponse.errors()[0].error().code(), -32600);
    QCOMPARE(assignedErrorResponse.errors()[0].error().message(), QString("Invalid Request"));
    QCOMPARE(assignedErrorResponse.errors()[1].id().toString(), QString("err-2"));
    QCOMPARE(assignedErrorResponse.errors()[1].error().code(), -32601);
    QCOMPARE(assignedErrorResponse.errors()[1].error().message(), QString("Method not found"));
}

QTEST_MAIN(tst_QMcpJSONRPCBatchResponse)
#include "tst_qmcpjsonrpcbatchresponse.moc"