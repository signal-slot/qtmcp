// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpImplementation>
#include <QtTest/QTest>

class tst_QMcpImplementation : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void settersAndGetters();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpImplementation::defaultValues()
{
    QMcpImplementation impl;
    QVERIFY(impl.name().isEmpty());
    QVERIFY(impl.version().isEmpty());
}

void tst_QMcpImplementation::settersAndGetters()
{
    QMcpImplementation impl;

    // Test name property
    impl.setName("TestImpl");
    QCOMPARE(impl.name(), "TestImpl");

    // Test version property
    impl.setVersion("1.0.0");
    QCOMPARE(impl.version(), "1.0.0");

    // Test that setting same value doesn't change anything
    impl.setName("TestImpl");
    QCOMPARE(impl.name(), "TestImpl");
    impl.setVersion("1.0.0");
    QCOMPARE(impl.version(), "1.0.0");

    // Test changing values
    impl.setName("NewImpl");
    QCOMPARE(impl.name(), "NewImpl");
    impl.setVersion("2.0.0");
    QCOMPARE(impl.version(), "2.0.0");
}

void tst_QMcpImplementation::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Basic test
    QTest::newRow("basic") << R"({
        "name": "TestImpl",
        "version": "1.0.0"
    })"_ba
    << QVariantMap {
        { "name", "TestImpl" },
        { "version", "1.0.0" }
    };

    // Test with empty strings
    QTest::newRow("empty strings") << R"({
        "name": "",
        "version": ""
    })"_ba
    << QVariantMap {
        { "name", "" },
        { "version", "" }
    };

    // Test with special characters
    QTest::newRow("special chars") << R"({
        "name": "Test-Impl_123",
        "version": "1.0.0-beta.1+build.123"
    })"_ba
    << QVariantMap {
        { "name", "Test-Impl_123" },
        { "version", "1.0.0-beta.1+build.123" }
    };
}

void tst_QMcpImplementation::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpImplementation impl;
    QVERIFY(impl.fromJsonObject(object));

    // Verify properties
    QCOMPARE(impl.name(), data.value("name").toString());
    QCOMPARE(impl.version(), data.value("version").toString());

    // Verify JSON conversion
    const auto converted = impl.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpImplementation::copy_data()
{
    convert_data();
}

void tst_QMcpImplementation::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpImplementation impl;
    QVERIFY(impl.fromJsonObject(object));

    // Test copy constructor
    QMcpImplementation impl2(impl);
    QCOMPARE(impl2.name(), data.value("name").toString());
    QCOMPARE(impl2.version(), data.value("version").toString());

    // Test assignment operator
    QMcpImplementation impl3;
    impl3 = impl2;
    QCOMPARE(impl3.name(), data.value("name").toString());
    QCOMPARE(impl3.version(), data.value("version").toString());
}

QTEST_MAIN(tst_QMcpImplementation)
#include "tst_qmcpimplementation.moc"
