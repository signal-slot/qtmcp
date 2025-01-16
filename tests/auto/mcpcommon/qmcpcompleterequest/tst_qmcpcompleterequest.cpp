// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpCompleteRequest>
#include <QtTest/QTest>

class tst_QMcpCompleteRequest : public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpCompleteRequest::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with prompt reference
    QTest::newRow("prompt reference") << R"({
        "id": 1,
        "params": {
            "argument": {
                "name": "input",
                "value": "test content"
            },
            "ref": {
                "type": "ref/prompt",
                "name": "code_completion"
            }
        }
    })"_ba
    << QVariantMap {
        { "id", 1 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "completion/complete"_L1 },
        { "params", QVariantMap {
            { "argument", QVariantMap {
                { "name", "input"_L1 },
                { "value", "test content"_L1 }
            }},
            { "ref", QVariantMap {
                { "type", "ref/prompt"_L1 },
                { "name", "code_completion"_L1 }
            }}
        }}
    };

    // Test with resource reference
    QTest::newRow("resource reference") << R"({
        "id": 2,
        "params": {
            "argument": {
                "name": "query",
                "value": "search term"
            },
            "ref": {
                "type": "ref/resource",
                "uri": "search://database/items"
            }
        }
    })"_ba
    << QVariantMap {
        { "id", 2 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "completion/complete"_L1 },
        { "params", QVariantMap {
            { "argument", QVariantMap {
                { "name", "query"_L1 },
                { "value", "search term"_L1 }
            }},
            { "ref", QVariantMap {
                { "type", "ref/resource"_L1 },
                { "uri", "search://database/items"_L1 }
            }}
        }}
    };
}

void tst_QMcpCompleteRequest::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpCompleteRequest request;
    QVERIFY(request.fromJsonObject(object));
    TestHelper::verify(&request, data);

    const auto objectConverted = request.toJsonObject();
    QCOMPARE(objectConverted, QJsonObject::fromVariantMap(data));
}

void tst_QMcpCompleteRequest::copy_data()
{
    convert_data();
}

void tst_QMcpCompleteRequest::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpCompleteRequest request;
    QVERIFY(request.fromJsonObject(object));

    QMcpCompleteRequest request2(request);
    TestHelper::verify(&request2, data);

    QMcpCompleteRequest request3;
    request3 = request2;
    TestHelper::verify(&request3, data);
}

QTEST_MAIN(tst_QMcpCompleteRequest)
#include "tst_qmcpcompleterequest.moc"
