// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpjsonrpcbatchrequest.h>
#include <QtMcpCommon/qmcppingrequest.h>
#include <QtMcpCommon/qmcpcreatemessagerequest.h>
#include <QtMcpCommon/qmcpcalltoolrequest.h>
#include <QtTest/QTest>

class tst_QMcpJSONRPCBatchRequest : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void jsonConversion();
    void copyTest();
};

void tst_QMcpJSONRPCBatchRequest::defaultValues()
{
    // Test default values for QMcpJSONRPCBatchRequest
    QMcpJSONRPCBatchRequest genericRequest;
    QVERIFY(genericRequest.requests().isEmpty());
    QCOMPARE(genericRequest.jsonrpc(), QString("2.0"));
}

void tst_QMcpJSONRPCBatchRequest::jsonConversion()
{
    // Create a batch of JSON-RPC requests using QMcpPingRequest
    QMcpJSONRPCBatchRequest batchRequest;
    QList<QMcpJSONRPCRequest *> requests;
    
    // Create first request - QMcpPingRequest
    QMcpPingRequest pingRequest;
    pingRequest.setId(1);
    requests.append(&pingRequest);

    // Create second request - another QMcpPingRequest with a string ID
    QMcpPingRequest pingRequest2;
    pingRequest2.setId("req-2");
    requests.append(&pingRequest2);
    
    batchRequest.setRequests(requests);

    // Verify the batch request
    requests = batchRequest.requests();
    QCOMPARE(requests.size(), 2);
    auto request = requests.at(0);
    QCOMPARE(request->id().toInt(), 1);
    QCOMPARE(request->method(), QString("ping"));
    request = requests.at(1);
    QCOMPARE(request->id().toString(), QString("req-2"));
    QCOMPARE(request->method(), QString("ping"));
    
    // Test JSON conversion
    QJsonObject jsonObj = batchRequest.toJsonObject();
    QCOMPARE(jsonObj["jsonrpc"].toString(), QString("2.0"));
    QVERIFY(jsonObj.contains("requests"));
    QVERIFY(jsonObj["requests"].isArray());
    QJsonArray requestsArray = jsonObj["requests"].toArray();
    QCOMPARE(requestsArray.size(), 2);
    
    // Verify first request in JSON
    QJsonObject req1Json = requestsArray[0].toObject();
    QCOMPARE(req1Json["id"].toInt(), 1);
    QCOMPARE(req1Json["method"].toString(), QString("ping"));
    
    // Verify second request in JSON
    QJsonObject req2Json = requestsArray[1].toObject();
    QCOMPARE(req2Json["id"].toString(), QString("req-2"));
    QCOMPARE(req2Json["method"].toString(), QString("ping"));
}

void tst_QMcpJSONRPCBatchRequest::copyTest()
{
    // Create a batch request with non-empty requests using concrete request types
    QMcpJSONRPCBatchRequest batchRequest;
    QList<QMcpJSONRPCRequest *> requests;
    
    // Create first request - QMcpPingRequest
    QMcpPingRequest pingRequest;
    pingRequest.setId(1);
    QMcpPingRequestParams pingParams;
    pingRequest.setParams(pingParams);
    requests.append(&pingRequest);
    
    // Create second request - another QMcpPingRequest with a string ID
    QMcpPingRequest pingRequest2;
    pingRequest2.setId("req-2");
    requests.append(&pingRequest2);
    
    batchRequest.setRequests(requests);
    
    // Test copy constructor
    QMcpJSONRPCBatchRequest copiedRequest(batchRequest);
    requests = copiedRequest.requests();
    QCOMPARE(requests.size(), 2);
    auto request = requests.at(0);
    QCOMPARE(request->id().toInt(), 1);
    QCOMPARE(request->method(), QString("ping"));
    request = requests.at(1);
    QCOMPARE(request->id().toString(), QString("req-2"));
    QCOMPARE(request->method(), QString("ping"));
    
    // Test assignment operator
    QMcpJSONRPCBatchRequest assignedRequest;
    assignedRequest = batchRequest;
    requests = assignedRequest.requests();
    QCOMPARE(requests.size(), 2);
    request = requests.at(0);
    QCOMPARE(request->id().toInt(), 1);
    QCOMPARE(request->method(), QString("ping"));
    request = requests.at(1);
    QCOMPARE(request->id().toString(), QString("req-2"));
    QCOMPARE(request->method(), QString("ping"));
}

QTEST_MAIN(tst_QMcpJSONRPCBatchRequest)
#include "tst_qmcpjsonrpcbatchrequest.moc"
