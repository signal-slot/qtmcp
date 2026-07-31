// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QJsonObject>
#include <QtMcpCommon/QMcpClientCapabilities>
#include <QtMcpCommon/QMcpTasksExtensionCapability>
#include <QtMcpCommon/qtmcpnamespace.h>
#include <QtTest/QTest>

class tst_QMcpTasksExtensionCapability : public QObject
{
    Q_OBJECT

private slots:
    void identifier();
    void serializesToAnEmptyObject();
    void announcedThroughClientCapabilities();
};

void tst_QMcpTasksExtensionCapability::identifier()
{
    QCOMPARE(QMcpTasksExtensionCapability::identifier(),
             "io.modelcontextprotocol/tasks"_L1);
}

void tst_QMcpTasksExtensionCapability::serializesToAnEmptyObject()
{
    // The extension defines no settings, and its schema forbids any member at
    // all, so support is signalled by presence alone.
    QMcpTasksExtensionCapability capability;
    QCOMPARE(capability.toJsonObject(), QJsonObject {});

    // The identifier is a static function rather than a property precisely so
    // that it cannot leak into the serialized form.
    QVERIFY(!capability.toJsonObject().contains("identifier"_L1));

    QVERIFY(capability.fromJsonObject(QJsonObject {}));
    QCOMPARE(capability.toJsonObject(), QJsonObject {});
}

void tst_QMcpTasksExtensionCapability::announcedThroughClientCapabilities()
{
    // The extensions member is an untyped map keyed by extension identifier, so
    // this is how a client declares the capability a server needs before it may
    // answer with a task handle.
    QMcpClientCapabilities capabilities;
    capabilities.setExtensions({
        { QMcpTasksExtensionCapability::identifier(),
          QMcpTasksExtensionCapability().toJsonObject() }
    });

    const auto object = capabilities.toJsonObject(QtMcp::ProtocolVersion::v2026_07_28);
    const auto extensions = object.value("extensions"_L1).toObject();
    QVERIFY(extensions.contains("io.modelcontextprotocol/tasks"_L1));
    QCOMPARE(extensions.value("io.modelcontextprotocol/tasks"_L1).toObject(), QJsonObject {});
}

QTEST_MAIN(tst_QMcpTasksExtensionCapability)
#include "tst_qmcptasksextensioncapability.moc"
