// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtMcpCommon/QMcpInputRequiredResult>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpInputRequiredResult : public QObject
{
    Q_OBJECT

private slots:
    void defaultResultType();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
    void versionGating();
};

void tst_QMcpInputRequiredResult::defaultResultType()
{
    // The type exists to carry exactly one resultType, so it must not have to
    // be set by hand.
    QMcpInputRequiredResult result;
    QCOMPARE(result.resultType(), "input_required"_L1);
}

void tst_QMcpInputRequiredResult::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // An interim result asking the client to elicit something, plus the
    // opaque token the client has to echo back on the retry.
    QTest::newRow("input requests") << R"({
        "resultType": "input_required",
        "inputRequests": {
            "elicit-1": {
                "method": "elicitation/create",
                "params": {
                    "message": "Which file should be opened?",
                    "requestedSchema": {
                        "type": "object",
                        "properties": {
                            "path": { "type": "string" }
                        }
                    }
                }
            }
        },
        "requestState": "8f14e45fceea167a"
    })"_ba
    << QVariantMap {
        { "resultType", "input_required"_L1 },
        { "inputRequests", QVariantMap {
            { "elicit-1", QVariantMap {
                { "method", "elicitation/create"_L1 },
                { "params", QVariantMap {
                    { "message", "Which file should be opened?"_L1 },
                    { "requestedSchema", QVariantMap {
                        { "type", "object"_L1 },
                        { "properties", QVariantMap {
                            { "path", QVariantMap { { "type", "string"_L1 } } }
                        }}
                    }}
                }}
            }}
        }},
        { "requestState", "8f14e45fceea167a"_L1 }
    };

    // The schema marks both members optional; a result that only resumes a
    // previously answered round trip carries just the state.
    QTest::newRow("request state only") << R"({
        "resultType": "input_required",
        "requestState": "8f14e45fceea167a"
    })"_ba
    << QVariantMap {
        { "resultType", "input_required"_L1 },
        { "requestState", "8f14e45fceea167a"_L1 }
    };
}

void tst_QMcpInputRequiredResult::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpInputRequiredResult result;
    QVERIFY(result.fromJsonObject(doc.object()));
    TestHelper::verify(&result, data);

    // The result has to survive a round trip through JSON unchanged.
    QCOMPARE(result.toJsonObject(), QJsonObject::fromVariantMap(data));
}

void tst_QMcpInputRequiredResult::copy_data()
{
    convert_data();
}

void tst_QMcpInputRequiredResult::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    const auto doc = QJsonDocument::fromJson(json, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    QMcpInputRequiredResult result;
    QVERIFY(result.fromJsonObject(doc.object()));

    const auto expected = QJsonObject::fromVariantMap(data);

    QMcpInputRequiredResult copyConstructed(result);
    TestHelper::verify(&copyConstructed, data);
    QCOMPARE(copyConstructed.toJsonObject(), expected);

    QMcpInputRequiredResult assigned;
    assigned = copyConstructed;
    TestHelper::verify(&assigned, data);
    QCOMPARE(assigned.toJsonObject(), expected);

    // Modifying the original must not affect the copies.
    result.setRequestState("0000000000000000"_L1);
    result.setInputRequests(QJsonObject { { "changed"_L1, QJsonObject {} } });
    QCOMPARE(copyConstructed.toJsonObject(), expected);
    QCOMPARE(assigned.toJsonObject(), expected);
}

void tst_QMcpInputRequiredResult::versionGating()
{
    QMcpInputRequiredResult result;
    result.setRequestState("8f14e45fceea167a"_L1);

    // resultType has been added in 2026-07-28 and must not leak into an older
    // revision of the protocol, even though it is what identifies this result.
    const auto oldObject = result.toJsonObject(QtMcp::ProtocolVersion::v2025_11_25);
    QVERIFY(!oldObject.contains("resultType"_L1));
    QCOMPARE(oldObject.value("requestState"_L1).toString(), "8f14e45fceea167a"_L1);

    const auto newObject = result.toJsonObject(QtMcp::ProtocolVersion::v2026_07_28);
    QCOMPARE(newObject.value("resultType"_L1).toString(), "input_required"_L1);
}

QTEST_MAIN(tst_QMcpInputRequiredResult)
#include "tst_qmcpinputrequiredresult.moc"
