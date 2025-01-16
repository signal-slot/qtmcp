// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpResource>
#include <QtTest/QTest>

class tst_QMcpResource : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpResource::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Basic resource with required properties only
    QTest::newRow("basic") << R"({
        "name": "test.txt",
        "uri": "file:///path/to/test.txt"
    })"_ba
    << QVariantMap {
        { "name", "test.txt" },
        { "uri", "file:///path/to/test.txt" }
    };

    // Resource with all properties
    QTest::newRow("full") << R"({
        "name": "document.txt",
        "uri": "file:///path/to/document.txt",
        "description": "Important document",
        "mimeType": "text/plain",
        "size": 1024,
        "annotations": {
            "audience": ["user", "assistant"],
            "priority": 0.8
        }
    })"_ba
    << QVariantMap {
        { "name", "document.txt" },
        { "uri", "file:///path/to/document.txt" },
        { "description", "Important document" },
        { "mimeType", "text/plain" },
        { "size", 1024 },
        { "annotations", QVariantMap {
            { "audience", QVariantList {
                QVariant::fromValue(QMcpRole::user),
                QVariant::fromValue(QMcpRole::assistant),
            }},
            { "priority", 0.8 }
        }}
    };

    // Resource with empty optional properties
    QTest::newRow("empty optionals") << R"({
        "name": "empty.txt",
        "uri": "file:///path/to/empty.txt",
        "description": "",
        "mimeType": "",
        "size": 0,
        "annotations": {
            "audience": [],
            "priority": 0.0
        }
    })"_ba
    << QVariantMap {
        { "name", "empty.txt" },
        { "uri", "file:///path/to/empty.txt" },
    };

    // Resource with Windows-style path
    QTest::newRow("windows path") << R"({
        "name": "windows.txt",
        "uri": "file:///C:/Users/Test/windows.txt",
        "mimeType": "text/plain",
        "size": 512
    })"_ba
    << QVariantMap {
        { "name", "windows.txt" },
        { "uri", "file:///C:/Users/Test/windows.txt" },
        { "mimeType", "text/plain" },
        { "size", 512 }
    };
}

void tst_QMcpResource::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpResource resource;
    QVERIFY(resource.fromJsonObject(object));
    TestHelper::verify(&resource, data);

    // Verify conversion
    const auto converted = resource.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpResource::copy_data()
{
    convert_data();
}

void tst_QMcpResource::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpResource resource;
    QVERIFY(resource.fromJsonObject(object));

    // Test copy constructor
    QMcpResource resource2(resource);
    QCOMPARE(resource2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpResource resource3;
    resource3 = resource2;
    QCOMPARE(resource3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpResource)
#include "tst_qmcpresource.moc"
