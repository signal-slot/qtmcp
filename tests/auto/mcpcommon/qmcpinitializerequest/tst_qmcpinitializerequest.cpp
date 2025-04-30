// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpInitializeRequest>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpInitializeRequest : public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpInitializeRequest::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with full capabilities
    QTest::newRow("full capabilities") << R"({
        "id": 1,
        "params": {
            "capabilities": {
                "experimental": {
                    "additionalProperties": {
                        "feature1": true,
                        "feature2": {
                            "nested": "value"
                        }
                    }
                },
                "roots": {
                    "listChanged": true
                },
                "sampling": {
                    "additionalProperties": {
                        "stream": true,
                        "options": {
                            "temperature": 0.7
                        }
                    }
                }
            },
            "clientInfo": {
                "name": "TestClient",
                "version": "1.0.0"
            },
            "protocolVersion": "2024-11-05"
        }
    })"_ba
    << QVariantMap {
        { "id", 1 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "initialize"_L1 },
        { "params", QVariantMap {
            { "capabilities", QVariantMap {
                { "experimental", QVariantMap {
                    { "additionalProperties", QVariantMap {
                        { "feature1", true },
                        { "feature2", QVariantMap {
                            { "nested", "value"_L1 }
                        }}
                    }}
                }},
                { "roots", QVariantMap {
                    { "listChanged", true }
                }},
                { "sampling", QVariantMap {
                    { "additionalProperties", QVariantMap {
                        { "stream", true },
                        { "options", QVariantMap {
                            { "temperature", 0.7 }
                        }}
                    }}
                }}
            }},
            { "clientInfo", QVariantMap {
                { "name", "TestClient"_L1 },
                { "version", "1.0.0"_L1 }
            }},
            { "protocolVersion", "2024-11-05"_L1 }
        }}
    };

    // Test with minimal capabilities
    QTest::newRow("minimal capabilities") << R"({
        "id": 2,
        "params": {
            "capabilities": {
                "experimental": {},
                "roots": {
                    "listChanged": false
                }
            },
            "clientInfo": {
                "name": "MinimalClient",
                "version": "0.1.0"
            },
            "protocolVersion": "2024-11-05"
        }
    })"_ba
    << QVariantMap {
        { "id", 2 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "initialize"_L1 },
        { "params", QVariantMap {
            { "capabilities", QVariantMap {
            }},
            { "clientInfo", QVariantMap {
                { "name", "MinimalClient"_L1 },
                { "version", "0.1.0"_L1 }
            }},
            { "protocolVersion", "2024-11-05"_L1 }
        }}
    };
}

void tst_QMcpInitializeRequest::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpInitializeRequest request;
    QVERIFY(request.fromJsonObject(object));
    TestHelper::verify(&request, data);

    const auto objectConverted = request.toJsonObject();
    QCOMPARE(objectConverted, QJsonObject::fromVariantMap(data));
}

void tst_QMcpInitializeRequest::copy_data()
{
    convert_data();
}

void tst_QMcpInitializeRequest::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpInitializeRequest request;
    QVERIFY(request.fromJsonObject(object));

    QMcpInitializeRequest request2(request);
    TestHelper::verify(&request2, data);

    QMcpInitializeRequest request3;
    request3 = request2;
    TestHelper::verify(&request3, data);
}

QTEST_MAIN(tst_QMcpInitializeRequest)
#include "tst_qmcpinitializerequest.moc"
