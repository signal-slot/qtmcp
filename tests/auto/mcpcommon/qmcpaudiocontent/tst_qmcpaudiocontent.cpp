// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpAnnotations>
#include <QtMcpCommon/QMcpAudioContent>
#include <QtTest/QTest>

class tst_QMcpAudioContent : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void setters();
};

void tst_QMcpAudioContent::defaultValues()
{
    QMcpAudioContent content;
    QCOMPARE(content.type(), "audio"_L1);
    QCOMPARE(content.data(), QByteArray());
    QCOMPARE(content.mimeType(), QString());
    QVERIFY(content.annotations().audience().isEmpty());
    QCOMPARE(content.annotations().priority(), 0.0);
}

void tst_QMcpAudioContent::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Basic audio content
    QTest::newRow("basic") << R"({
        "type": "audio",
        "data": "SGVsbG8gV29ybGQ=",
        "mimeType": "audio/mp3"
    })"_ba
    << QVariantMap {
        { "type", "audio" },
        { "data", QByteArray("SGVsbG8gV29ybGQ=") },
        { "mimeType", "audio/mp3" }
    };

    // Audio content with annotations
    QTest::newRow("with annotations") << R"({
        "type": "audio",
        "data": "SGVsbG8gV29ybGQ=",
        "mimeType": "audio/wav",
        "annotations": {
            "audience": ["user", "assistant"],
            "priority": 0.8
        }
    })"_ba
    << QVariantMap {
        { "type", "audio" },
        { "data", QByteArray("SGVsbG8gV29ybGQ=") },
        { "mimeType", "audio/wav" },
        { "annotations", QVariantMap {
            { "audience", QVariantList {
                QVariant::fromValue(QMcpRole::user),
                QVariant::fromValue(QMcpRole::assistant),
            }},
            { "priority", 0.8 }
        }}
    };

    // Audio content with empty data
    QTest::newRow("empty data") << R"({
        "type": "audio",
        "data": "",
        "mimeType": "audio/mp3"
    })"_ba
    << QVariantMap {
        { "type", "audio" },
        { "data", ""_ba },
        { "mimeType", "audio/mp3" }
    };
}

void tst_QMcpAudioContent::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpAudioContent content;
    QVERIFY(content.fromJsonObject(object));
    TestHelper::verify(&content, data);

    // Replace empty QByteArray into empty QString for comparison
    for (auto &value : data) {
        if (value.canConvert<QByteArray>() && value.toByteArray().isEmpty()) {
            value = QString();
        }
    }

    // Verify conversion
    const auto converted = content.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpAudioContent::copy_data()
{
    convert_data();
}

void tst_QMcpAudioContent::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpAudioContent content;
    QVERIFY(content.fromJsonObject(object));

    // Replace empty QByteArray into empty QString for comparison
    for (auto &value : data) {
        if (value.canConvert<QByteArray>() && value.toByteArray().isEmpty()) {
            value = QString();
        }
    }

    // Test copy constructor
    QMcpAudioContent content2(content);
    QCOMPARE(content2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpAudioContent content3;
    content3 = content2;
    QCOMPARE(content3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpAudioContent::setters()
{
    QMcpAudioContent content;
    
    // Test data setter
    QByteArray testData("Test audio data");
    content.setData(testData);
    QCOMPARE(content.data(), testData);
    
    // Test mimeType setter
    QString testMimeType("audio/ogg");
    content.setMimeType(testMimeType);
    QCOMPARE(content.mimeType(), testMimeType);
    
    // Test annotations setter
    QMcpAnnotations annotations;
    QList<QMcpRole::QMcpRole> audience;
    audience.append(QMcpRole::user);
    annotations.setAudience(audience);
    annotations.setPriority(0.5);
    content.setAnnotations(annotations);
    QCOMPARE(content.annotations().audience().size(), 1);
    QCOMPARE(content.annotations().audience().first(), QMcpRole::user);
    QCOMPARE(content.annotations().priority(), 0.5);
    
    // Test that type is constant and cannot be changed
    QCOMPARE(content.type(), "audio"_L1);
}

QTEST_MAIN(tst_QMcpAudioContent)
#include "tst_qmcpaudiocontent.moc"
