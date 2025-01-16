// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtMcpCommon/qmcpnotification.h>
#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtTest/QTest>

// Concrete test class implementing QMcpNotification
class TestNotification : public QMcpNotification
{
public:
    TestNotification() : QMcpNotification() {}

    // Implement pure virtual method from QMcpJSONRPCNotification
    QString method() const override { return "test.method"; }
};

class tst_QMcpNotification : public QObject
{
    Q_OBJECT

private slots:
};

QTEST_MAIN(tst_QMcpNotification)
#include "tst_qmcpnotification.moc"
