// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpAnnotated>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpAnnotated : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void versionSpecificSerialization_data();
    void versionSpecificSerialization();
    void versionSpecificDeserialization_data();
    void versionSpecificDeserialization();
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
        "audience": ["assistant"]
    })"_ba
    << QVariantMap {
        { "audience", QVariantList {
            QVariant::fromValue(QMcpRole::assistant)
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
    })"_ba
    << QVariantMap {};
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

void tst_QMcpAnnotated::versionSpecificSerialization_data()
{
    QTest::addColumn<QtMcp::ProtocolVersion>("protocolVersion");
    QTest::addColumn<bool>("hasAnnotations");
    QTest::addColumn<bool>("shouldIncludeAnnotations");

    // Latest version (2025-03-26) should include annotations when present
    QTest::newRow("2025-03-26 with annotations") << QtMcp::ProtocolVersion::v2025_03_26 << true << true;
    QTest::newRow("2025-03-26 without annotations") << QtMcp::ProtocolVersion::v2025_03_26 << false << false;

    // Previous version (2024-11-05) should always omit annotations
    QTest::newRow("2024-11-05 with annotations") << QtMcp::ProtocolVersion::v2024_11_05 << true << false;
    QTest::newRow("2024-11-05 without annotations") << QtMcp::ProtocolVersion::v2024_11_05 << false << false;

    // Latest version should default to latest behavior
    QTest::newRow("Latest with annotations") << QtMcp::ProtocolVersion::Latest << true << true;
    QTest::newRow("Latest without annotations") << QtMcp::ProtocolVersion::Latest << false << false;
}

void tst_QMcpAnnotated::versionSpecificSerialization()
{
    QFETCH(QtMcp::ProtocolVersion, protocolVersion);
    QFETCH(bool, hasAnnotations);
    QFETCH(bool, shouldIncludeAnnotations);

    // Create an annotated object
    QMcpAnnotated annotated;

    if (hasAnnotations) {
        QMcpAnnotations annotations;
        annotations.setAudience({QMcpRole::assistant});
        annotations.setPriority(0.8);
        annotated.setAnnotations(annotations);
    }

    // Serialize with the specified protocol version
    QJsonObject jsonObj = annotated.toJsonObject(protocolVersion);

    // Verify whether annotations are included based on the protocol version
    if (shouldIncludeAnnotations) {
        QVERIFY(jsonObj.contains("annotations"));
        QVERIFY(jsonObj.value("annotations").isObject());

        if (hasAnnotations) {
            QJsonObject anns = jsonObj.value("annotations").toObject();
            QVERIFY(anns.contains("audience"));
            QVERIFY(anns.contains("priority"));
            QCOMPARE(anns.value("priority").toDouble(), 0.8);
        } else {
            QVERIFY(jsonObj.value("annotations").toObject().isEmpty());
        }
    } else {
        QVERIFY(!jsonObj.contains("annotations"));
    }
}

void tst_QMcpAnnotated::versionSpecificDeserialization_data()
{
    QTest::addColumn<QtMcp::ProtocolVersion>("protocolVersion");
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<bool>("shouldHaveAnnotations");

    // JSON with annotations
    QByteArray jsonWithAnnotations = R"({
        "annotations": {
            "audience": ["assistant"],
            "priority": 0.8
        }
    })"_ba;

    // JSON without annotations
    QByteArray jsonWithoutAnnotations = R"({})"_ba;

    // Latest version (2025-03-26) should process annotations when present
    QTest::newRow("2025-03-26 with annotations") << QtMcp::ProtocolVersion::v2025_03_26 << jsonWithAnnotations << true;
    QTest::newRow("2025-03-26 without annotations") << QtMcp::ProtocolVersion::v2025_03_26 << jsonWithoutAnnotations << false;

    // Previous version (2024-11-05) should ignore annotations even if present
    QTest::newRow("2024-11-05 with annotations") << QtMcp::ProtocolVersion::v2024_11_05 << jsonWithAnnotations << false;
    QTest::newRow("2024-11-05 without annotations") << QtMcp::ProtocolVersion::v2024_11_05 << jsonWithoutAnnotations << false;

    // Latest version should default to latest behavior
    QTest::newRow("Latest with annotations") << QtMcp::ProtocolVersion::Latest << jsonWithAnnotations << true;
    QTest::newRow("Latest without annotations") << QtMcp::ProtocolVersion::Latest << jsonWithoutAnnotations << false;
}

void tst_QMcpAnnotated::versionSpecificDeserialization()
{
    QFETCH(QtMcp::ProtocolVersion, protocolVersion);
    QFETCH(QByteArray, json);
    QFETCH(bool, shouldHaveAnnotations);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);

    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpAnnotated annotated;
    QVERIFY(annotated.fromJsonObject(object, protocolVersion));

    if (shouldHaveAnnotations) {
        QVERIFY(!annotated.annotations().audience().isEmpty() || annotated.annotations().priority() != 0);

        // If we have annotations from JSON, verify they are correct
        if (object.contains("annotations")) {
            if (object.value("annotations").toObject().contains("audience")) {
                QCOMPARE(annotated.annotations().audience().size(), 1);
                QCOMPARE(annotated.annotations().audience().first(), QMcpRole::assistant);
            }

            if (object.value("annotations").toObject().contains("priority")) {
                QCOMPARE(annotated.annotations().priority(), 0.8);
            }
        }
    } else {
        QVERIFY(annotated.annotations().audience().isEmpty());
        QCOMPARE(annotated.annotations().priority(), 0.0);
    }
}

QTEST_MAIN(tst_QMcpAnnotated)
#include "tst_qmcpannotated.moc"
