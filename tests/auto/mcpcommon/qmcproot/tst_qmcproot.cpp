// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpRoot>
#include <QtTest/QTest>

class tst_QMcpRoot : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpRoot::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Basic root with name
    QTest::newRow("with name") << R"({
        "name": "Test Root",
        "uri": "file:///path/to/root"
    })"_ba
    << QVariantMap {
        { "name", "Test Root" },
        { "uri", "file:///path/to/root" }
    };

    // Root without name
    QTest::newRow("without name") << R"({
        "uri": "file:///path/to/root"
    })"_ba
    << QVariantMap {
        { "uri", "file:///path/to/root" }
    };

    // Root with empty name
    QTest::newRow("empty name") << R"({
        "name": "",
        "uri": "file:///path/to/root"
    })"_ba
    << QVariantMap {
        { "uri", "file:///path/to/root" }
    };

    // Root with special characters in path
    QTest::newRow("special chars") << R"({
        "name": "Test Root",
        "uri": "file:///path/with spaces/and-dashes/root"
    })"_ba
    << QVariantMap {
        { "name", "Test Root" },
        { "uri", "file:///path/with spaces/and-dashes/root" }
    };

    // Root with Windows-style path
    QTest::newRow("windows path") << R"({
        "name": "Windows Root",
        "uri": "file:///C:/Users/Test/Documents"
    })"_ba
    << QVariantMap {
        { "name", "Windows Root" },
        { "uri", "file:///C:/Users/Test/Documents" }
    };
}

void tst_QMcpRoot::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpRoot root;
    QVERIFY(root.fromJsonObject(object));
    TestHelper::verify(&root, data);

    // Verify conversion
    const auto converted = root.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpRoot::copy_data()
{
    convert_data();
}

void tst_QMcpRoot::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpRoot root;
    QVERIFY(root.fromJsonObject(object));

    // Test copy constructor
    QMcpRoot root2(root);
    TestHelper::verify(&root2, data);

    // Test assignment operator
    QMcpRoot root3;
    root3 = root2;
    TestHelper::verify(&root3, data);
}

QTEST_MAIN(tst_QMcpRoot)
#include "tst_qmcproot.moc"
