// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpAnnotated>
#include <QtTest/QTest>

class tst_QMcpAnnotated : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpAnnotated::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with both audience and priority
    QTest::newRow("full annotations") << R"({
        "annotations": {
            "audience": ["assistant", "user"],
            "priority": 0.8
        }
    })"_ba
    << QVariantMap {
        { "annotations", QVariantMap {
            { "audience", QVariantList { 
                QVariant::fromValue(QMcpRole::assistant),
                QVariant::fromValue(QMcpRole::user)
            }},
            { "priority", 0.8 }
        }}
    };

    // Test with only audience
    QTest::newRow("only audience") << R"({
        "annotations": {
            "audience": ["assistant"]
        }
    })"_ba
    << QVariantMap {
        { "annotations", QVariantMap {
            { "audience", QVariantList { 
                QVariant::fromValue(QMcpRole::assistant)
            }}
        }}
    };

    // Test with only priority
    QTest::newRow("only priority") << R"({
        "annotations": {
            "priority": 1.0
        }
    })"_ba
    << QVariantMap {
        { "annotations", QVariantMap {
            { "priority", 1.0 }
        }}
    };

    // Test with empty annotations
    QTest::newRow("empty annotations") << R"({
        "annotations": {}
    })"_ba
    << QVariantMap {
        { "annotations", QVariantMap {} }
    };
}

void tst_QMcpAnnotated::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpAnnotated annotated;
    QVERIFY(annotated.fromJsonObject(object));
    TestHelper::verify(&annotated, data);
}

void tst_QMcpAnnotated::copy_data()
{
    convert_data();
}

void tst_QMcpAnnotated::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpAnnotated annotated;
    QVERIFY(annotated.fromJsonObject(object));

    QMcpAnnotated annotated2(annotated);
    TestHelper::verify(&annotated2, data);

    QMcpAnnotated annotated3;
    annotated3 = annotated2;
    TestHelper::verify(&annotated3, data);
}

QTEST_MAIN(tst_QMcpAnnotated)
#include "tst_qmcpannotated.moc"
