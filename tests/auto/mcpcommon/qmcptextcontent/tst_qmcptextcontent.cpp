// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpAnnotations>
#include <QtMcpCommon/QMcpTextContent>
#include <QtTest/QTest>

class tst_QMcpTextContent : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpTextContent::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Basic text content
    QTest::newRow("basic") << R"({
        "text": "Hello, world!",
        "type": "text"
    })"_ba
    << QVariantMap {
        { "text", "Hello, world!" },
        { "type", "text"_ba }
    };

    // Text content with annotations
    QTest::newRow("with annotations") << R"({
        "text": "Important message",
        "type": "text",
        "annotations": {
            "audience": ["user", "assistant"],
            "priority": 0.8
        }
    })"_ba
    << QVariantMap {
        { "text", "Important message" },
        { "type", "text"_ba },
        { "annotations", QVariantMap {
            { "audience", QVariantList {
                QVariant::fromValue(QMcpRole::user),
                QVariant::fromValue(QMcpRole::assistant),
            }},
            { "priority", 0.8 }
        }}
    };

    // Text content with empty annotations
    QTest::newRow("empty annotations") << R"({
        "text": "Simple message",
        "type": "text",
        "annotations": {
            "audience": [],
            "priority": 0.0
        }
    })"_ba
    << QVariantMap {
        { "text", "Simple message" },
        { "type", "text"_ba },
    };

    // Text content with special characters
    QTest::newRow("special chars") << R"({
        "text": "Line 1\nLine 2\tTabbed",
        "type": "text"
    })"_ba
    << QVariantMap {
        { "text", "Line 1\nLine 2\tTabbed" },
        { "type", "text"_ba }
    };
}

void tst_QMcpTextContent::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpTextContent content;
    QVERIFY(content.fromJsonObject(object));
    TestHelper::verify(&content, data);

    // Verify conversion
    const auto converted = content.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpTextContent::copy_data()
{
    convert_data();
}

void tst_QMcpTextContent::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpTextContent content;
    QVERIFY(content.fromJsonObject(object));

    // Test copy constructor
    QMcpTextContent content2(content);
    QCOMPARE(content2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpTextContent content3;
    content3 = content2;
    QCOMPARE(content3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpTextContent)
#include "tst_qmcptextcontent.moc"
