// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/qmcpresourcelink.h>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpResourceLink : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void versionGating();
};

void tst_QMcpResourceLink::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Only the required members: type, uri and name.
    QTest::newRow("minimal") << R"({
        "type": "resource_link",
        "uri": "file:///project/src/main.cpp",
        "name": "main.cpp"
    })"_ba
    << QVariantMap {
        { "type", "resource_link"_ba },
        { "uri", "file:///project/src/main.cpp"_L1 },
        { "name", "main.cpp"_L1 }
    };

    // Every member the 2025-06-18 revision defines.
    QTest::newRow("full") << R"({
        "type": "resource_link",
        "uri": "file:///project/src/main.cpp",
        "name": "main.cpp",
        "title": "Application entry point",
        "description": "The file holding the main function",
        "mimeType": "text/x-c++src",
        "size": 1024,
        "_meta": {
            "generatedBy": "tst_qmcpresourcelink"
        }
    })"_ba
    << QVariantMap {
        { "type", "resource_link"_ba },
        { "uri", "file:///project/src/main.cpp"_L1 },
        { "name", "main.cpp"_L1 },
        { "title", "Application entry point"_L1 },
        { "description", "The file holding the main function"_L1 },
        { "mimeType", "text/x-c++src"_L1 },
        { "size", 1024 },
        { "_meta", QVariantMap {
            { "generatedBy", "tst_qmcpresourcelink"_L1 }
        }}
    };
}

void tst_QMcpResourceLink::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpResourceLink link;
    QVERIFY(link.fromJsonObject(object));
    TestHelper::verify(&link, data);

    // The link has to survive a round trip through JSON unchanged.
    QCOMPARE(link.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpResourceLink::copy_data()
{
    convert_data();
}

void tst_QMcpResourceLink::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpResourceLink link;
    QVERIFY(link.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpResourceLink copyConstructed(link);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpResourceLink assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    link.setName("other.cpp"_L1);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpResourceLink::versionGating()
{
    QMcpResourceLink link;
    link.setUri(QUrl("file:///project/src/main.cpp"_L1));
    link.setName("main.cpp"_L1);
    link.setTitle("Application entry point"_L1);
    link.setMeta(QJsonObject { { "generatedBy"_L1, "tst_qmcpresourcelink"_L1 } });

    // title and _meta have been added in 2025-06-18 and must not leak into an
    // older revision of the protocol.
    const auto oldObject = link.toJsonObject(QtMcp::ProtocolVersion::v2025_03_26);
    QVERIFY(!oldObject.contains("title"_L1));
    QVERIFY(!oldObject.contains("_meta"_L1));
    // The members that already existed are still there.
    QCOMPARE(oldObject.value("type"_L1).toString(), "resource_link"_L1);
    QCOMPARE(oldObject.value("uri"_L1).toString(), "file:///project/src/main.cpp"_L1);
    QCOMPARE(oldObject.value("name"_L1).toString(), "main.cpp"_L1);

    const auto newObject = link.toJsonObject(QtMcp::ProtocolVersion::v2025_06_18);
    QCOMPARE(newObject.value("title"_L1).toString(), "Application entry point"_L1);
    QCOMPARE(newObject.value("_meta"_L1).toObject().value("generatedBy"_L1).toString(),
             "tst_qmcpresourcelink"_L1);
}

QTEST_MAIN(tst_QMcpResourceLink)
#include "tst_qmcpresourcelink.moc"
