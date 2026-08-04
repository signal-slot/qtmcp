// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/qmcpelicitresult.h>
#include <QtTest/QTest>

class tst_QMcpElicitResult : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void missingAction();
};

void tst_QMcpElicitResult::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // The user filled in the form, so content is present.
    QTest::newRow("accept") << R"({
        "action": "accept",
        "content": {
            "agree": true,
            "nickname": "tester",
            "age": 30,
            "color": "green"
        }
    })"_ba
    << QVariantMap {
        { "resultType", "complete"_L1 },
        { "action", "accept"_L1 },
        { "content", QVariantMap {
            { "agree", true },
            { "nickname", "tester"_L1 },
            { "age", 30 },
            { "color", "green"_L1 }
        }}
    };

    // The user declined, so there is no content at all.
    QTest::newRow("decline") << R"({
        "action": "decline"
    })"_ba
    << QVariantMap {
        { "resultType", "complete"_L1 },
        { "action", "decline"_L1 }
    };

    // The user dismissed the dialog without choosing.
    QTest::newRow("cancel") << R"({
        "action": "cancel"
    })"_ba
    << QVariantMap {
        { "resultType", "complete"_L1 },
        { "action", "cancel"_L1 }
    };
}

void tst_QMcpElicitResult::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpElicitResult result;
    QVERIFY(result.fromJsonObject(doc.object()));
    TestHelper::verify(&result, data);

    const auto converted = result.toJsonObject();
    QCOMPARE(converted, QJsonObject::fromVariantMap(data));
    // content must not be invented for the non-accept actions.
    QCOMPARE(converted.contains("content"_L1), data.contains("content"));
}

void tst_QMcpElicitResult::copy_data()
{
    convert_data();
}

void tst_QMcpElicitResult::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpElicitResult result;
    QVERIFY(result.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpElicitResult copyConstructed(result);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpElicitResult assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    result.setAction("modified"_L1);
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpElicitResult::missingAction()
{
    // action is required, so an object without it cannot be parsed.
    QMcpElicitResult result;
    QVERIFY(!result.fromJsonObject(QJsonObject { { "content"_L1, QJsonObject {} } }));
}

QTEST_MAIN(tst_QMcpElicitResult)
#include "tst_qmcpelicitresult.moc"
