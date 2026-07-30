// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/qmcpicon.h>
#include <QtTest/QTest>

class tst_QMcpIcon : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void srcIsRequired();
};

void tst_QMcpIcon::defaultValues()
{
    const QMcpIcon icon;
    QVERIFY(icon.src().isEmpty());
    QVERIFY(icon.mimeType().isEmpty());
    QVERIFY(icon.sizes().isEmpty());
    QVERIFY(icon.theme().isEmpty());
}

void tst_QMcpIcon::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // src is the only required member.
    QTest::newRow("minimal") << R"({
        "src": "https://example.invalid/icon.png"
    })"_ba
    << QVariantMap {
        { "src", "https://example.invalid/icon.png"_L1 }
    };

    // Every member the 2025-11-25 revision defines.
    QTest::newRow("full") << R"({
        "src": "https://example.invalid/icon.svg",
        "mimeType": "image/svg+xml",
        "sizes": ["48x48", "96x96"],
        "theme": "dark"
    })"_ba
    << QVariantMap {
        { "src", "https://example.invalid/icon.svg"_L1 },
        { "mimeType", "image/svg+xml"_L1 },
        { "sizes", QStringList { "48x48"_L1, "96x96"_L1 } },
        { "theme", "dark"_L1 }
    };

    // "any" is the size of a scalable icon.
    QTest::newRow("scalable") << R"({
        "src": "https://example.invalid/icon.svg",
        "sizes": ["any"],
        "theme": "light"
    })"_ba
    << QVariantMap {
        { "src", "https://example.invalid/icon.svg"_L1 },
        { "sizes", QStringList { "any"_L1 } },
        { "theme", "light"_L1 }
    };

    // src may also be a data: URI carrying the image inline.
    QTest::newRow("data uri") << R"({
        "src": "data:image/png;base64,iVBORw0KGgo=",
        "mimeType": "image/png"
    })"_ba
    << QVariantMap {
        { "src", "data:image/png;base64,iVBORw0KGgo="_L1 },
        { "mimeType", "image/png"_L1 }
    };
}

void tst_QMcpIcon::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpIcon icon;
    QVERIFY(icon.fromJsonObject(object));
    TestHelper::verify(&icon, data);

    // The icon has to survive a round trip through JSON unchanged.
    QCOMPARE(icon.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpIcon::copy_data()
{
    convert_data();
}

void tst_QMcpIcon::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpIcon icon;
    QVERIFY(icon.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpIcon copyConstructed(icon);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpIcon assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    icon.setSrc(QUrl("https://example.invalid/other.png"_L1));
    icon.setSizes({ "16x16"_L1 });
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpIcon::srcIsRequired()
{
    // An icon without a source cannot be displayed, so the member is REQUIRED.
    QMcpIcon icon;
    QVERIFY(!icon.fromJsonObject(QJsonObject { { "mimeType"_L1, "image/png"_L1 } }));

    // It is serialized even when it is empty, for the same reason.
    QVERIFY(QMcpIcon().toJsonObject().contains("src"_L1));
}

QTEST_MAIN(tst_QMcpIcon)
#include "tst_qmcpicon.moc"
