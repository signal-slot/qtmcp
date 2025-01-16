// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtTest/QTest>
#include <QtMcpCommon/qmcpjsonrpcnotification.h>
#include <QtMcpCommon/qmcpjsonrpcnotificationparams.h>
#include "../testhelper.h"

// Concrete test class implementing QMcpJSONRPCNotification
class TestJSONRPCNotification : public QMcpJSONRPCNotification
{
public:
    TestJSONRPCNotification() : QMcpJSONRPCNotification() {}

    // Implement pure virtual method
    QString method() const override { return "test.method"; }
};

class tst_QMcpJSONRPCNotification : public QObject
{
    Q_OBJECT

private slots:
};

QTEST_MAIN(tst_QMcpJSONRPCNotification)
#include "tst_qmcpjsonrpcnotification.moc"
