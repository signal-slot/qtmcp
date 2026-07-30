// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpSubscriptionsListenRequest>
#include <QtTest/QTest>

class tst_QMcpSubscriptionsListenRequest : public QObject
{
    Q_OBJECT

private slots:
    void methodName();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void filterSurvivesTheRequest();
};

void tst_QMcpSubscriptionsListenRequest::methodName()
{
    QMcpSubscriptionsListenRequest request;
    QCOMPARE(request.method(), "subscriptions/listen"_L1);
}

void tst_QMcpSubscriptionsListenRequest::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // A stream that only asks for tool list changes.
    QTest::newRow("tools only") << R"({
        "id": 1,
        "params": {
            "notifications": {
                "toolsListChanged": true
            }
        }
    })"_ba
    << QVariantMap {
        { "id", 1 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "subscriptions/listen"_L1 },
        { "params", QVariantMap {
            { "notifications", QVariantMap {
                { "toolsListChanged", true }
            }}
        }}
    };

    // Every opt-in the filter offers, including the resource URIs that
    // replaced resources/subscribe.
    QTest::newRow("everything") << R"({
        "id": "listen-1",
        "params": {
            "notifications": {
                "toolsListChanged": true,
                "promptsListChanged": true,
                "resourcesListChanged": true,
                "resourceSubscriptions": ["file:///project/README.md"]
            }
        }
    })"_ba
    << QVariantMap {
        { "id", "listen-1"_L1 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "subscriptions/listen"_L1 },
        { "params", QVariantMap {
            { "notifications", QVariantMap {
                { "toolsListChanged", true },
                { "promptsListChanged", true },
                { "resourcesListChanged", true },
                { "resourceSubscriptions", QStringList { "file:///project/README.md"_L1 } }
            }}
        }}
    };

    // params and its notifications member are both required, so an empty
    // filter still has to be serialized as an object.
    QTest::newRow("no opt-in") << R"({
        "id": 2,
        "params": {
            "notifications": {}
        }
    })"_ba
    << QVariantMap {
        { "id", 2 },
        { "jsonrpc", "2.0"_L1 },
        { "method", "subscriptions/listen"_L1 },
        { "params", QVariantMap {
            { "notifications", QVariantMap {} }
        }}
    };
}

void tst_QMcpSubscriptionsListenRequest::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpSubscriptionsListenRequest request;
    QVERIFY(request.fromJsonObject(doc.object()));
    TestHelper::verify(&request, data);

    // The request has to survive a round trip through JSON unchanged.
    QCOMPARE(request.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpSubscriptionsListenRequest::copy_data()
{
    convert_data();
}

void tst_QMcpSubscriptionsListenRequest::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpSubscriptionsListenRequest request;
    QVERIFY(request.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpSubscriptionsListenRequest copyConstructed(request);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpSubscriptionsListenRequest assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    auto params = request.params();
    auto notifications = params.notifications();
    notifications.setPromptsListChanged(!notifications.promptsListChanged());
    params.setNotifications(notifications);
    request.setParams(params);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpSubscriptionsListenRequest::filterSurvivesTheRequest()
{
    // The filter a caller assembles by hand has to reach the wire unchanged;
    // this is how QMcpClient sends the opt-ins.
    QMcpSubscriptionFilter filter;
    filter.setToolsListChanged(true);
    filter.setResourceSubscriptions({ "file:///project/README.md"_L1 });

    QMcpSubscriptionsListenRequestParams params;
    params.setNotifications(filter);

    QMcpSubscriptionsListenRequest request;
    request.setId(7);
    request.setParams(params);

    QCOMPARE(request.params().notifications().toolsListChanged(), true);

    const auto object = request.toJsonObject();
    QCOMPARE(object.value("method"_L1).toString(), "subscriptions/listen"_L1);
    const auto notifications = object.value("params"_L1).toObject().value("notifications"_L1).toObject();
    QCOMPARE(notifications.value("toolsListChanged"_L1).toBool(), true);
    // Types the client did not opt in to must not appear at all.
    QVERIFY(!notifications.contains("promptsListChanged"_L1));
    QCOMPARE(notifications.value("resourceSubscriptions"_L1).toArray().size(), 1);
}

QTEST_MAIN(tst_QMcpSubscriptionsListenRequest)
#include "tst_qmcpsubscriptionslistenrequest.moc"
