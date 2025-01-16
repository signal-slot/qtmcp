// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpListPromptsRequest>
#include <QtTest/QTest>

class tst_QMcpListPromptsRequest : public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpListPromptsRequest::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");
    QTest::newRow("1") << "{ \"id\": 1, \"params\": { \"cursor\": \"cursor1\" } }"_ba
                       << QVariantMap {
                              { "id", 1 },
                              { "jsonrpc", "2.0"_L1 },
                              { "method", "prompts/list"_L1 },
                              { "params", QVariantMap { { "cursor", "cursor1" } } }
                          };
}

void tst_QMcpListPromptsRequest::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);

    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpListPromptsRequest request;
    QVERIFY(request.fromJsonObject(object));
    TestHelper::verify(&request, data);

    const auto objectConverted = request.toJsonObject();
    QCOMPARE(objectConverted, QJsonObject::fromVariantMap(data));
}

void tst_QMcpListPromptsRequest::copy_data()
{
    convert_data();
}

void tst_QMcpListPromptsRequest::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);

    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpListPromptsRequest request;
    QVERIFY(request.fromJsonObject(object));

    QMcpListPromptsRequest request2(request);
    TestHelper::verify(&request2, data);

    QMcpListPromptsRequest request3;
    request3 = request2;
    TestHelper::verify(&request3, data);
}

QTEST_MAIN(tst_QMcpListPromptsRequest)
#include "tst_qmcplistpromptsrequest.moc"
