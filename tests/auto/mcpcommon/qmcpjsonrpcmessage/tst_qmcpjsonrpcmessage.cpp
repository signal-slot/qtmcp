// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonValue>
#include <QtMcpCommon/qmcpjsonrpcerror.h>
#include <QtMcpCommon/qmcpjsonrpcresponse.h>
#include <QtMcpCommon/qmcpresult.h>
#include <QtTest/QTest>
#include <variant>

using QMcpJSONRPCMessageTest = std::variant<
    QMcpJSONRPCResponse,
    QMcpJSONRPCError,
    QJsonValue,
    std::nullptr_t
>;

class tst_QMcpJSONRPCMessage : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void responseVariant();
    void errorVariant();
    void jsonValueVariant();
    void nullptrVariant();
    void variantAssignment();
    void variantVisit();
};

void tst_QMcpJSONRPCMessage::defaultValues()
{
    QMcpJSONRPCMessageTest message = nullptr;
    QVERIFY(std::holds_alternative<std::nullptr_t>(message));
}

void tst_QMcpJSONRPCMessage::responseVariant()
{
    // Create a response with result
    QMcpJSONRPCResponse response;
    response.setId(123); // Set numeric ID
    
    QMcpResult result;
    QJsonObject resultProps{{"key", "value"}};
    result.setAdditionalProperties(resultProps);
    response.setResult(result);
    
    // Create message from response
    QMcpJSONRPCMessageTest message = response;
    QVERIFY(std::holds_alternative<QMcpJSONRPCResponse>(message));
    
    const auto& storedResponse = std::get<QMcpJSONRPCResponse>(message);
    QCOMPARE(storedResponse.jsonrpc(), QString("2.0"));
    QCOMPARE(storedResponse.id().toInt(), 123);
    QCOMPARE(storedResponse.result().additionalProperties()["key"].toString(), QString("value"));

    // Test with string ID
    response.setId("response-1");
    message = response;
    QCOMPARE(std::get<QMcpJSONRPCResponse>(message).id().toString(), QString("response-1"));
}

void tst_QMcpJSONRPCMessage::errorVariant()
{
    // Create an error
    QMcpJSONRPCError error;
    QMcpJSONRPCErrorError errorDetails;
    errorDetails.setCode(-32600);
    errorDetails.setMessage("Invalid Request");
    QJsonObject errorData{{"details", "Additional error info"}};
    errorDetails.setData(errorData);
    error.setError(errorDetails);
    
    // Create message from error
    QMcpJSONRPCMessageTest message = error;
    QVERIFY(std::holds_alternative<QMcpJSONRPCError>(message));
    
    const auto& storedError = std::get<QMcpJSONRPCError>(message);
    QCOMPARE(storedError.jsonrpc(), QString("2.0"));
    QCOMPARE(storedError.error().code(), -32600);
    QCOMPARE(storedError.error().message(), QString("Invalid Request"));
    QCOMPARE(storedError.error().data().toObject()["details"].toString(), QString("Additional error info"));
}

void tst_QMcpJSONRPCMessage::jsonValueVariant()
{
    // Test different JSON value types
    QJsonObject obj{{"test", "value"}};
    QMcpJSONRPCMessageTest objMessage = QJsonValue(obj);
    QVERIFY(std::holds_alternative<QJsonValue>(objMessage));
    QCOMPARE(std::get<QJsonValue>(objMessage).toObject()["test"].toString(), QString("value"));

    QJsonArray arr{"item1", "item2"};
    QMcpJSONRPCMessageTest arrMessage = QJsonValue(arr);
    QVERIFY(std::holds_alternative<QJsonValue>(arrMessage));
    QCOMPARE(std::get<QJsonValue>(arrMessage).toArray()[0].toString(), QString("item1"));

    QString str = "test string";
    QMcpJSONRPCMessageTest strMessage = QJsonValue(str);
    QVERIFY(std::holds_alternative<QJsonValue>(strMessage));
    QCOMPARE(std::get<QJsonValue>(strMessage).toString(), str);

    double num = 42.5;
    QMcpJSONRPCMessageTest numMessage = QJsonValue(num);
    QVERIFY(std::holds_alternative<QJsonValue>(numMessage));
    QCOMPARE(std::get<QJsonValue>(numMessage).toDouble(), num);

    bool flag = true;
    QMcpJSONRPCMessageTest boolMessage = QJsonValue(flag);
    QVERIFY(std::holds_alternative<QJsonValue>(boolMessage));
    QCOMPARE(std::get<QJsonValue>(boolMessage).toBool(), flag);

    QMcpJSONRPCMessageTest nullMessage = QJsonValue(QJsonValue::Null);
    QVERIFY(std::holds_alternative<QJsonValue>(nullMessage));
    QVERIFY(std::get<QJsonValue>(nullMessage).isNull());
}

void tst_QMcpJSONRPCMessage::nullptrVariant()
{
    QMcpJSONRPCMessageTest message = nullptr;
    QVERIFY(std::holds_alternative<std::nullptr_t>(message));
}

void tst_QMcpJSONRPCMessage::variantAssignment()
{
    QMcpJSONRPCMessageTest message = nullptr;

    // Test assignment of each variant type
    QMcpJSONRPCResponse response;
    response.setId(123);
    QMcpResult result;
    result.setAdditionalProperties(QJsonObject{{"key", "value"}});
    response.setResult(result);
    message = response;
    QVERIFY(std::holds_alternative<QMcpJSONRPCResponse>(message));

    QMcpJSONRPCError error;
    QMcpJSONRPCErrorError errorDetails;
    errorDetails.setCode(-32600);
    errorDetails.setMessage("Invalid Request");
    error.setError(errorDetails);
    message = error;
    QVERIFY(std::holds_alternative<QMcpJSONRPCError>(message));

    QJsonValue jsonValue("test");
    message = jsonValue;
    QVERIFY(std::holds_alternative<QJsonValue>(message));

    message = nullptr;
    QVERIFY(std::holds_alternative<std::nullptr_t>(message));
}

void tst_QMcpJSONRPCMessage::variantVisit()
{
    QMcpJSONRPCMessageTest message = nullptr;

    // Test visitor pattern with lambda
    auto visitor = [](const auto& value) -> QString {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, QMcpJSONRPCResponse>) {
            return "response";
        } else if constexpr (std::is_same_v<T, QMcpJSONRPCError>) {
            return "error";
        } else if constexpr (std::is_same_v<T, QJsonValue>) {
            return "json";
        } else {
            return "null";
        }
    };

    // Test visit with each variant type
    QMcpJSONRPCResponse response;
    response.setId(123);
    QMcpResult result;
    result.setAdditionalProperties(QJsonObject{{"key", "value"}});
    response.setResult(result);
    message = response;
    QCOMPARE(std::visit(visitor, message), QString("response"));

    QMcpJSONRPCError error;
    QMcpJSONRPCErrorError errorDetails;
    errorDetails.setCode(-32600);
    errorDetails.setMessage("Invalid Request");
    error.setError(errorDetails);
    message = error;
    QCOMPARE(std::visit(visitor, message), QString("error"));

    message = QJsonValue("test");
    QCOMPARE(std::visit(visitor, message), QString("json"));

    message = nullptr;
    QCOMPARE(std::visit(visitor, message), QString("null"));
}

QTEST_MAIN(tst_QMcpJSONRPCMessage)
#include "tst_qmcpjsonrpcmessage.moc"
