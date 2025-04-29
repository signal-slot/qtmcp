// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpAnnotations>
#include <QtTest/QTest>

class tst_QMcpAnnotations : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void defaultValues();
    void priorityBounds();
    void audienceOperations();
};

void tst_QMcpAnnotations::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Test with both audience and priority
    QTest::newRow("full annotations") << R"({
        "audience": ["assistant", "user"],
        "priority": 0.8
    })"_ba
                                      << QVariantMap {
                                             { "audience", QVariantList {
                                                              QVariant::fromValue(QMcpRole::assistant),
                                                              QVariant::fromValue(QMcpRole::user)
                                                          }},
                                             { "priority", 0.8 }
                                         };

    // Test with only audience
    QTest::newRow("only audience") << R"({
        "audience": ["user"]
    })"_ba
                                   << QVariantMap {
                                                  { "audience", QVariantList {
            QVariant::fromValue(QMcpRole::user)
}}
};

// Test with only priority
QTest::newRow("only priority") << R"({
        "priority": 1.0
    })"_ba
                               << QVariantMap {
                                      { "priority", 1.0 }
                                  };

// Test with empty object
QTest::newRow("empty") << R"({})"_ba
                       << QVariantMap {
                          };

// Test with all roles in audience
QTest::newRow("all roles") << R"({
        "audience": ["assistant", "user"],
        "priority": 0.5
    })"_ba
                           << QVariantMap {
                                  { "audience", QVariantList {
                                                   QVariant::fromValue(QMcpRole::assistant),
                                                   QVariant::fromValue(QMcpRole::user)
                                               }},
                                  { "priority", 0.5 }
                              };

// Test with extreme priority values
QTest::newRow("max priority") << R"({
        "priority": 1.0
    })"_ba
                             << QVariantMap {
                                    { "priority", 1.0 }
                                };

QTest::newRow("min priority") << R"({
        "priority": 0.0
    })"_ba
                             << QVariantMap {
                                    { "priority", 0.0 }
                                };

// Test with out-of-range priority (should be clamped to valid range)
QTest::newRow("beyond max priority") << R"({
        "priority": 2.0
    })"_ba
                                    << QVariantMap {
                                           { "priority", 1.0 }
                                       };

QTest::newRow("negative priority") << R"({
        "priority": -0.5
    })"_ba
                                  << QVariantMap {
                                         { "priority", 0.0 }
                                     };
}

void tst_QMcpAnnotations::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpAnnotations annotations;
    QVERIFY(annotations.fromJsonObject(object));
    TestHelper::verify(&annotations, data);

    const auto objectConverted = annotations.toJsonObject();
    const auto expectedObject = QJsonObject::fromVariantMap(data);
    QCOMPARE(objectConverted, expectedObject);
}

void tst_QMcpAnnotations::copy_data()
{
    convert_data();
}

void tst_QMcpAnnotations::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpAnnotations annotations;
    QVERIFY(annotations.fromJsonObject(object));

    QMcpAnnotations annotations2(annotations);
    TestHelper::verify(&annotations2, data);

    QMcpAnnotations annotations3;
    annotations3 = annotations2;
    TestHelper::verify(&annotations3, data);
}

void tst_QMcpAnnotations::defaultValues()
{
    QMcpAnnotations annotations;

    // Check default values
    QVERIFY(annotations.audience().isEmpty());
    QCOMPARE(annotations.priority(), 0.0);
}

void tst_QMcpAnnotations::priorityBounds()
{
    QMcpAnnotations annotations;
    
    // Test valid priority values
    annotations.setPriority(0.0);
    QCOMPARE(annotations.priority(), 0.0);
    
    annotations.setPriority(0.5);
    QCOMPARE(annotations.priority(), 0.5);
    
    annotations.setPriority(1.0);
    QCOMPARE(annotations.priority(), 1.0);
    
    // Test priority value clamping
    annotations.setPriority(-0.1);
    QCOMPARE(annotations.priority(), 0.0);
    
    annotations.setPriority(1.1);
    QCOMPARE(annotations.priority(), 1.0);
}

void tst_QMcpAnnotations::audienceOperations()
{
    QMcpAnnotations annotations;
    
    // Test adding roles to audience
    QVERIFY(annotations.audience().isEmpty());
    
    annotations.setAudience({QMcpRole::assistant});
    QCOMPARE(annotations.audience().size(), 1);
    QVERIFY(annotations.audience().contains(QMcpRole::assistant));
    
    // Test replacing audience
    annotations.setAudience({QMcpRole::user, QMcpRole::assistant});
    QCOMPARE(annotations.audience().size(), 2);
    QVERIFY(annotations.audience().contains(QMcpRole::user));
    QVERIFY(annotations.audience().contains(QMcpRole::assistant));
    
    // Test clearing audience
    annotations.setAudience({});
    QVERIFY(annotations.audience().isEmpty());
}

QTEST_MAIN(tst_QMcpAnnotations)
#include "tst_qmcpannotations.moc"
