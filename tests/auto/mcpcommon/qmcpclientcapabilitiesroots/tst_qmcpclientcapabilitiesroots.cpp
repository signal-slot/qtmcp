// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpClientCapabilitiesRoots>
#include <QtTest/QTest>

class tst_QMcpClientCapabilitiesRoots : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpClientCapabilitiesRoots::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Default value (false)
    QTest::newRow("default") << R"({
    })"_ba
    << QVariantMap {
    };

    // Enabled
    QTest::newRow("enabled") << R"({
        "listChanged": true
    })"_ba
    << QVariantMap {
        { "listChanged", true }
    };
}

void tst_QMcpClientCapabilitiesRoots::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpClientCapabilitiesRoots capabilities;
    QVERIFY(capabilities.fromJsonObject(object));
    TestHelper::verify(&capabilities, data);

    // Verify conversion
    const auto converted = capabilities.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpClientCapabilitiesRoots::copy_data()
{
    convert_data();
}

void tst_QMcpClientCapabilitiesRoots::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpClientCapabilitiesRoots capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Test copy constructor
    QMcpClientCapabilitiesRoots capabilities2(capabilities);
    QCOMPARE(capabilities2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpClientCapabilitiesRoots capabilities3;
    capabilities3 = capabilities2;
    QCOMPARE(capabilities3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpClientCapabilitiesRoots)
#include "tst_qmcpclientcapabilitiesroots.moc"
