// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpSubscriptionFilter>
#include <QtTest/QTest>

class tst_QMcpSubscriptionFilter : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpSubscriptionFilter::defaultValues()
{
    // Every notification type is opt-in, so nothing may be enabled by default
    // and an untouched filter must not serialize any opt-in at all.
    QMcpSubscriptionFilter filter;
    QVERIFY(!filter.toolsListChanged());
    QVERIFY(!filter.promptsListChanged());
    QVERIFY(!filter.resourcesListChanged());
    QVERIFY(filter.resourceSubscriptions().isEmpty());
    QCOMPARE(filter.toJsonObject(), QJsonObject {});
}

void tst_QMcpSubscriptionFilter::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // A client that only cares about the tool list.
    QTest::newRow("tools only") << R"({
        "toolsListChanged": true
    })"_ba
    << QVariantMap {
        { "toolsListChanged", true }
    };

    // Resource updates replaced resources/subscribe, so the URIs travel in the
    // filter as well.
    QTest::newRow("resource subscriptions") << R"({
        "resourcesListChanged": true,
        "resourceSubscriptions": [
            "file:///project/README.md",
            "file:///project/src/main.cpp"
        ]
    })"_ba
    << QVariantMap {
        { "resourcesListChanged", true },
        { "resourceSubscriptions", QStringList {
            "file:///project/README.md"_L1,
            "file:///project/src/main.cpp"_L1
        }}
    };

    // Every member the type defines.
    QTest::newRow("everything") << R"({
        "toolsListChanged": true,
        "promptsListChanged": true,
        "resourcesListChanged": true,
        "resourceSubscriptions": ["file:///project/README.md"]
    })"_ba
    << QVariantMap {
        { "toolsListChanged", true },
        { "promptsListChanged", true },
        { "resourcesListChanged", true },
        { "resourceSubscriptions", QStringList { "file:///project/README.md"_L1 } }
    };

    // Explicitly declining every notification type is the same as the default,
    // and must not be mistaken for an opt-in.
    QTest::newRow("all declined") << R"({
        "toolsListChanged": false,
        "promptsListChanged": false,
        "resourcesListChanged": false
    })"_ba
    << QVariantMap {
    };
}

void tst_QMcpSubscriptionFilter::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpSubscriptionFilter filter;
    QVERIFY(filter.fromJsonObject(doc.object()));
    TestHelper::verify(&filter, data);

    // The filter has to survive a round trip through JSON unchanged.
    QCOMPARE(filter.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpSubscriptionFilter::copy_data()
{
    convert_data();
}

void tst_QMcpSubscriptionFilter::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpSubscriptionFilter filter;
    QVERIFY(filter.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpSubscriptionFilter copyConstructed(filter);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpSubscriptionFilter assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    filter.setToolsListChanged(!filter.toolsListChanged());
    filter.setResourceSubscriptions({ "file:///other"_L1 });
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

QTEST_MAIN(tst_QMcpSubscriptionFilter)
#include "tst_qmcpsubscriptionfilter.moc"
