// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpCallToolRequest>
#include <QtTest/QTest>

class tst_QMcpCallToolRequest : public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void defaultValues();
    void methodName();
};

void tst_QMcpCallToolRequest::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with complex arguments
    QTest::newRow("complex arguments") << R"({
        "id": 1,
        "params": {
            "name": "processImage",
            "arguments": {
                "width": 800,
                "height": 600,
                "format": "jpeg",
                "quality": 0.9,
                "filters": ["blur", "sharpen"],
                "metadata": {
                    "author": "Test User",
                    "timestamp": "2024-02-01T12:00:00Z"
                }
            }
        }
    })"_ba
    << QVariantMap {
        { "id", 1 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "tools/call"_L1 },
        { "params", QVariantMap {
            { "name", "processImage"_L1 },
            { "arguments", QVariantMap {
                { "width", 800 },
                { "height", 600 },
                { "format", "jpeg"_L1 },
                { "quality", 0.9 },
                { "filters", QVariantList { "blur"_L1, "sharpen"_L1 } },
                { "metadata", QVariantMap {
                    { "author", "Test User"_L1 },
                    { "timestamp", "2024-02-01T12:00:00Z"_L1 }
                }}
            }}
        }}
    };

    // Test with simple arguments
    QTest::newRow("simple arguments") << R"({
        "id": 2,
        "params": {
            "name": "echo",
            "arguments": {
                "message": "Hello, World!"
            }
        }
    })"_ba
    << QVariantMap {
        { "id", 2 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "tools/call"_L1 },
        { "params", QVariantMap {
            { "name", "echo"_L1 },
            { "arguments", QVariantMap {
                { "message", "Hello, World!"_L1 }
            }}
        }}
    };

    // Test without arguments
    QTest::newRow("no arguments") << R"({
        "id": 3,
        "params": {
            "name": "status"
        }
    })"_ba
    << QVariantMap {
        { "id", 3 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "tools/call"_L1 },
        { "params", QVariantMap {
            { "name", "status"_L1 }
        }}
    };

    // Test with empty arguments
    QTest::newRow("empty arguments") << R"({
        "id": 4,
        "params": {
            "name": "reset",
            "arguments": {
            }
        }
    })"_ba
    << QVariantMap {
        { "id", 4 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "tools/call"_L1 },
        { "params", QVariantMap {
            { "name", "reset"_L1 },
        }}
    };
}

void tst_QMcpCallToolRequest::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpCallToolRequest request;
    QVERIFY(request.fromJsonObject(object));
    TestHelper::verify(&request, data);

    const auto objectConverted = request.toJsonObject();
    const auto expectedObject = QJsonObject::fromVariantMap(data);
    QCOMPARE(objectConverted, expectedObject);
}

void tst_QMcpCallToolRequest::copy_data()
{
    convert_data();
}

void tst_QMcpCallToolRequest::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpCallToolRequest request;
    QVERIFY(request.fromJsonObject(object));

    // Test copy constructor
    QMcpCallToolRequest request2(request);
    QCOMPARE(request2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpCallToolRequest request3;
    request3 = request2;
    QCOMPARE(request3.toJsonObject(), QJsonObject::fromVariantMap(data));

}

void tst_QMcpCallToolRequest::defaultValues()
{
    QMcpCallToolRequest request;
    
    // Check default values
    QVERIFY(request.params().name().isEmpty());
}

void tst_QMcpCallToolRequest::methodName()
{
    QMcpCallToolRequest request;
    QCOMPARE(request.method(), "tools/call"_L1);
}

QTEST_MAIN(tst_QMcpCallToolRequest)
#include "tst_qmcpcalltoolrequest.moc"
