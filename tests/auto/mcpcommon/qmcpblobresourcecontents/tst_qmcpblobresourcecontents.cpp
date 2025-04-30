// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpBlobResourceContents>
#include <QtTest/QTest>

class tst_QMcpBlobResourceContents : public QObject
{
    Q_OBJECT
private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void defaultValues();
};

void tst_QMcpBlobResourceContents::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with all properties
    QTest::newRow("full contents") << R"({
        "blob": "SGVsbG8gV29ybGQ=",
        "mimeType": "text/plain",
        "uri": "file:///path/to/file.txt"
    })"_ba
    << QVariantMap {
        { "blob", "SGVsbG8gV29ybGQ="_ba },
        { "mimeType", "text/plain"_L1 },
        { "uri", QUrl("file:///path/to/file.txt") }
    };

    // Test with binary data and image mime type
    QTest::newRow("image data") << R"({
        "blob": "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8z8BQDwAEhQGAhKmMIQAAAABJRU5ErkJggg==",
        "mimeType": "image/png",
        "uri": "https://example.com/image.png"
    })"_ba
    << QVariantMap {
        { "blob", "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8z8BQDwAEhQGAhKmMIQAAAABJRU5ErkJggg=="_ba },
        { "mimeType", "image/png"_L1 },
        { "uri", QUrl("https://example.com/image.png") }
    };

    // Test without mime type
    QTest::newRow("no mime type") << R"({
        "blob": "dGVzdCBkYXRh",
        "uri": "data:text/plain;base64,dGVzdCBkYXRh"
    })"_ba
    << QVariantMap {
        { "blob", "dGVzdCBkYXRh"_ba },
        { "uri", QUrl("data:text/plain;base64,dGVzdCBkYXRh") }
    };

    // Test with relative URI
    QTest::newRow("relative uri") << R"({
        "blob": "Y29udGVudA==",
        "mimeType": "application/octet-stream",
        "uri": "resources/data.bin"
    })"_ba
    << QVariantMap {
        { "blob", "Y29udGVudA=="_ba },
        { "mimeType", "application/octet-stream"_L1 },
        { "uri", QUrl("resources/data.bin") }
    };
}

void tst_QMcpBlobResourceContents::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpBlobResourceContents contents;
    QVERIFY(contents.fromJsonObject(object));
    TestHelper::verify(&contents, data);

    const auto objectConverted = contents.toJsonObject();
    const auto expectedObject = QJsonObject::fromVariantMap(data);
    QCOMPARE(objectConverted, expectedObject);
}

void tst_QMcpBlobResourceContents::copy_data()
{
    convert_data();
}

void tst_QMcpBlobResourceContents::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpBlobResourceContents contents;
    QVERIFY(contents.fromJsonObject(object));

    QMcpBlobResourceContents contents2(contents);
    // Compare required properties
    QCOMPARE(contents2.blob(), data.value("blob").toByteArray());
    QCOMPARE(contents2.uri(), data.value("uri").toUrl());
    // Compare optional mimeType if present
    if (data.contains("mimeType")) {
        QCOMPARE(contents2.mimeType(), data.value("mimeType").toString());
    } else {
        QVERIFY(contents2.mimeType().isEmpty());
    }

    QMcpBlobResourceContents contents3;
    contents3 = contents2;
    // Compare required properties
    QCOMPARE(contents3.blob(), data.value("blob").toByteArray());
    QCOMPARE(contents3.uri(), data.value("uri").toUrl());
    // Compare optional mimeType if present
    if (data.contains("mimeType")) {
        QCOMPARE(contents3.mimeType(), data.value("mimeType").toString());
    } else {
        QVERIFY(contents3.mimeType().isEmpty());
    }
}

void tst_QMcpBlobResourceContents::defaultValues()
{
    QMcpBlobResourceContents contents;

    // Check default values
    QVERIFY(contents.blob().isEmpty());
    QVERIFY(contents.mimeType().isEmpty());
    QVERIFY(contents.uri().isEmpty());
}

QTEST_MAIN(tst_QMcpBlobResourceContents)
#include "tst_qmcpblobresourcecontents.moc"
