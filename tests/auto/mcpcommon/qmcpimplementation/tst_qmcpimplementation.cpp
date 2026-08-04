// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpImplementation>
#include <QtMcpCommon/qmcpicon.h>
#include <QtMcpCommon/qtmcpnamespace.h>
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
    void versionGating();
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

void tst_QMcpImplementation::versionGating()
{
    QMcpIcon icon;
    icon.setSrc(QUrl("https://example.invalid/impl.png"_L1));

    QMcpImplementation impl;
    impl.setName("TestImpl"_L1);
    impl.setVersion("1.0.0"_L1);
    impl.setTitle("Test Implementation"_L1);
    impl.setDescription("An implementation used by the tests"_L1);
    impl.setWebsiteUrl(QUrl("https://example.invalid/"_L1));
    impl.setIcons({ icon });

    // title has been added in 2025-06-18, the rest in 2025-11-25; none of them
    // may leak into an older revision of the protocol.
    const auto oldObject = impl.toJsonObject(QtMcp::ProtocolVersion::v2025_03_26);
    QVERIFY(!oldObject.contains("title"_L1));
    QVERIFY(!oldObject.contains("description"_L1));
    QVERIFY(!oldObject.contains("websiteUrl"_L1));
    QVERIFY(!oldObject.contains("icons"_L1));
    // The members that already existed are still there.
    QCOMPARE(oldObject.value("name"_L1).toString(), "TestImpl"_L1);
    QCOMPARE(oldObject.value("version"_L1).toString(), "1.0.0"_L1);

    const auto object20250618 = impl.toJsonObject(QtMcp::ProtocolVersion::v2025_06_18);
    QCOMPARE(object20250618.value("title"_L1).toString(), "Test Implementation"_L1);
    QVERIFY(!object20250618.contains("description"_L1));
    QVERIFY(!object20250618.contains("websiteUrl"_L1));
    QVERIFY(!object20250618.contains("icons"_L1));

    const auto object20251125 = impl.toJsonObject(QtMcp::ProtocolVersion::v2025_11_25);
    QCOMPARE(object20251125.value("title"_L1).toString(), "Test Implementation"_L1);
    QCOMPARE(object20251125.value("description"_L1).toString(),
             "An implementation used by the tests"_L1);
    QCOMPARE(object20251125.value("websiteUrl"_L1).toString(), "https://example.invalid/"_L1);
    const auto icons = object20251125.value("icons"_L1).toArray();
    QCOMPARE(icons.size(), 1);
    QCOMPARE(icons.at(0).toObject().value("src"_L1).toString(),
             "https://example.invalid/impl.png"_L1);

    // Parsing follows the same rules: what the revision does not know is
    // ignored, the rest is read.
    const auto full = impl.toJsonObject();
    QMcpImplementation old;
    QVERIFY(old.fromJsonObject(full, QtMcp::ProtocolVersion::v2025_06_18));
    QCOMPARE(old.title(), "Test Implementation"_L1);
    QVERIFY(old.description().isEmpty());
    QVERIFY(old.websiteUrl().isEmpty());
    QVERIFY(old.icons().isEmpty());

    QMcpImplementation latest;
    QVERIFY(latest.fromJsonObject(full, QtMcp::ProtocolVersion::v2025_11_25));
    QCOMPARE(latest.description(), "An implementation used by the tests"_L1);
    QCOMPARE(latest.websiteUrl(), QUrl("https://example.invalid/"_L1));
    QCOMPARE(latest.icons().size(), 1);
    QCOMPARE(latest.icons().at(0).src(), QUrl("https://example.invalid/impl.png"_L1));
}

QTEST_MAIN(tst_QMcpImplementation)
#include "tst_qmcpimplementation.moc"
