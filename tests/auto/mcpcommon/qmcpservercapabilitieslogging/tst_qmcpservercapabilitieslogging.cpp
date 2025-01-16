// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpServerCapabilitiesLogging>
#include <QtTest/QTest>

class tst_QMcpServerCapabilitiesLogging : public QObject
{
    Q_OBJECT

private slots:
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpServerCapabilitiesLogging::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QVariantMap>("data");

    // Empty properties
    QTest::newRow("empty") << R"({
    })"_ba
    << QVariantMap {
    };

    // Simple properties
    QTest::newRow("simple") << R"({
        "additionalProperties": {
            "minLevel": "debug",
            "maxMessageSize": 1024
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "minLevel", "debug" },
            { "maxMessageSize", 1024 }
        }}
    };

    // Complex properties
    QTest::newRow("complex") << R"({
        "additionalProperties": {
            "levels": {
                "enabled": ["debug", "info", "warning", "error"],
                "default": "info",
                "config": {
                    "maxBufferSize": 10000,
                    "flushInterval": 1000
                }
            },
            "filters": {
                "modules": ["core", "network"],
                "patterns": ["*.error", "network.*"]
            }
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "levels", QVariantMap {
                { "enabled", QVariantList { "debug", "info", "warning", "error" } },
                { "default", "info" },
                { "config", QVariantMap {
                    { "maxBufferSize", 10000 },
                    { "flushInterval", 1000 }
                }}
            }},
            { "filters", QVariantMap {
                { "modules", QVariantList { "core", "network" } },
                { "patterns", QVariantList { "*.error", "network.*" } }
            }}
        }}
    };

    // Logging configuration with formatters
    QTest::newRow("formatters") << R"({
        "additionalProperties": {
            "formatters": {
                "default": {
                    "pattern": "[%d] %l: %m",
                    "dateFormat": "yyyy-MM-dd HH:mm:ss"
                },
                "json": {
                    "enabled": true,
                    "includeMetadata": true
                }
            },
            "outputs": ["console", "file"],
            "fileConfig": {
                "path": "/var/log/app.log",
                "maxSize": 10485760,
                "maxFiles": 5
            }
        }
    })"_ba
    << QVariantMap {
        { "additionalProperties", QVariantMap {
            { "formatters", QVariantMap {
                { "default", QVariantMap {
                    { "pattern", "[%d] %l: %m" },
                    { "dateFormat", "yyyy-MM-dd HH:mm:ss" }
                }},
                { "json", QVariantMap {
                    { "enabled", true },
                    { "includeMetadata", true }
                }}
            }},
            { "outputs", QVariantList { "console", "file" } },
            { "fileConfig", QVariantMap {
                { "path", "/var/log/app.log" },
                { "maxSize", 10485760 },
                { "maxFiles", 5 }
            }}
        }}
    };
}

void tst_QMcpServerCapabilitiesLogging::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpServerCapabilitiesLogging capabilities;
    QVERIFY(capabilities.fromJsonObject(object));
    TestHelper::verify(&capabilities, data);

    // Verify conversion
    const auto converted = capabilities.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(data);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpServerCapabilitiesLogging::copy_data()
{
    convert_data();
}

void tst_QMcpServerCapabilitiesLogging::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QVariantMap, data);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QMcpServerCapabilitiesLogging capabilities;
    QVERIFY(capabilities.fromJsonObject(object));

    // Test copy constructor
    QMcpServerCapabilitiesLogging capabilities2(capabilities);
    QCOMPARE(capabilities2.toJsonObject(), QJsonObject::fromVariantMap(data));

    // Test assignment operator
    QMcpServerCapabilitiesLogging capabilities3;
    capabilities3 = capabilities2;
    QCOMPARE(capabilities3.toJsonObject(), QJsonObject::fromVariantMap(data));
}

QTEST_MAIN(tst_QMcpServerCapabilitiesLogging)
#include "tst_qmcpservercapabilitieslogging.moc"
