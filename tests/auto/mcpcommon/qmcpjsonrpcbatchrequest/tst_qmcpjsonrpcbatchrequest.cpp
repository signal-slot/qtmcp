// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonValue>
#include <QtMcpCommon/qmcpjsonrpcbatchrequest.h>
#include <QtMcpCommon/qmcpjsonrpcrequest.h>
#include <QtMcpCommon/qmcpserverrequest.h>
#include <QtMcpCommon/qmcpclientrequest.h>
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

    // Test default values for QMcpServerJSONRPCBatchRequest
    QMcpServerJSONRPCBatchRequest serverRequest;
    QVERIFY(serverRequest.requests().isEmpty());
    QCOMPARE(serverRequest.jsonrpc(), QString("2.0"));

    // Test default values for QMcpClientJSONRPCBatchRequest
    QMcpClientJSONRPCBatchRequest clientRequest;
    QVERIFY(clientRequest.requests().isEmpty());
    QCOMPARE(clientRequest.jsonrpc(), QString("2.0"));
}

void tst_QMcpJSONRPCBatchRequest::jsonConversion()
{
    // Create a batch request with empty requests list
    QMcpJSONRPCBatchRequest batchRequest;
    
    // Test JSON conversion
    QJsonObject jsonObj = batchRequest.toJsonObject();
    QCOMPARE(jsonObj["jsonrpc"].toString(), QString("2.0"));
    QVERIFY(jsonObj.contains("requests"));
    QVERIFY(jsonObj["requests"].isArray());
    QCOMPARE(jsonObj["requests"].toArray().size(), 0);
    
    // Create a server batch request with empty requests list
    QMcpServerJSONRPCBatchRequest serverBatchRequest;
    
    // Test JSON conversion
    QJsonObject serverJsonObj = serverBatchRequest.toJsonObject();
    QCOMPARE(serverJsonObj["jsonrpc"].toString(), QString("2.0"));
    QVERIFY(serverJsonObj.contains("requests"));
    QVERIFY(serverJsonObj["requests"].isArray());
    QCOMPARE(serverJsonObj["requests"].toArray().size(), 0);
    
    // Create a client batch request with empty requests list
    QMcpClientJSONRPCBatchRequest clientBatchRequest;
    
    // Test JSON conversion
    QJsonObject clientJsonObj = clientBatchRequest.toJsonObject();
    QCOMPARE(clientJsonObj["jsonrpc"].toString(), QString("2.0"));
    QVERIFY(clientJsonObj.contains("requests"));
    QVERIFY(clientJsonObj["requests"].isArray());
    QCOMPARE(clientJsonObj["requests"].toArray().size(), 0);
}

void tst_QMcpJSONRPCBatchRequest::copyTest()
{
    // Create a batch request
    QMcpJSONRPCBatchRequest batchRequest;
    
    // Test copy constructor
    QMcpJSONRPCBatchRequest copiedRequest(batchRequest);
    QVERIFY(copiedRequest.requests().isEmpty());
    QCOMPARE(copiedRequest.jsonrpc(), QString("2.0"));
    
    // Test assignment operator
    QMcpJSONRPCBatchRequest assignedRequest;
    assignedRequest = batchRequest;
    QVERIFY(assignedRequest.requests().isEmpty());
    QCOMPARE(assignedRequest.jsonrpc(), QString("2.0"));
    
    // Create a server batch request
    QMcpServerJSONRPCBatchRequest serverBatchRequest;
    
    // Test copy constructor
    QMcpServerJSONRPCBatchRequest copiedServerRequest(serverBatchRequest);
    QVERIFY(copiedServerRequest.requests().isEmpty());
    QCOMPARE(copiedServerRequest.jsonrpc(), QString("2.0"));
    
    // Test assignment operator
    QMcpServerJSONRPCBatchRequest assignedServerRequest;
    assignedServerRequest = serverBatchRequest;
    QVERIFY(assignedServerRequest.requests().isEmpty());
    QCOMPARE(assignedServerRequest.jsonrpc(), QString("2.0"));
    
    // Create a client batch request
    QMcpClientJSONRPCBatchRequest clientBatchRequest;
    
    // Test copy constructor
    QMcpClientJSONRPCBatchRequest copiedClientRequest(clientBatchRequest);
    QVERIFY(copiedClientRequest.requests().isEmpty());
    QCOMPARE(copiedClientRequest.jsonrpc(), QString("2.0"));
    
    // Test assignment operator
    QMcpClientJSONRPCBatchRequest assignedClientRequest;
    assignedClientRequest = clientBatchRequest;
    QVERIFY(assignedClientRequest.requests().isEmpty());
    QCOMPARE(assignedClientRequest.jsonrpc(), QString("2.0"));
}

QTEST_MAIN(tst_QMcpJSONRPCBatchRequest)
#include "tst_qmcpjsonrpcbatchrequest.moc"