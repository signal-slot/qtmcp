// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QtTest/QTest>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

namespace TestHelper {

inline void verify(const QMcpGadget *gadget, const QVariantMap &data)
{
    const QMetaObject *mo = gadget->metaObject();
    for (int i = 0; i < mo->propertyCount(); i++) {
        const auto property = mo->property(i);
        const auto name = property.name();
        if (!data.contains(name))
            continue;
        const auto expectedValue = data.value(name);
        const auto value = property.readOnGadget(gadget);
        if (expectedValue.canConvert<QVariantMap>()) {
            switch (property.typeId()) {
            case QMetaType::QJsonObject:
                QCOMPARE(value.toJsonObject(), expectedValue.toJsonObject());
                break;
            case QMetaType::QJsonValue:
                QCOMPARE(value.toJsonValue(), expectedValue.toJsonValue());
                break;
            default:
                verify((const QMcpGadget *)value.constData(), data.value(name).toMap());
                break;
            }
        } else if (QByteArray(property.typeName()).startsWith("QList<")) {
            qDebug() << value << expectedValue;
            const auto valueList = value.toList();
            const auto expectedList = expectedValue.toList();
            QCOMPARE(valueList.count(), expectedList.count());
            for (int i = 0; i < valueList.count(); i++) {
                const auto value = valueList.at(i);
                const auto expectedValue = expectedList.at(i);
                if (expectedValue.canConvert<QVariantMap>()) {
                    switch (property.typeId()) {
                    case QMetaType::QJsonObject:
                        QCOMPARE(value.toJsonObject(), expectedValue.toJsonObject());
                        break;
                    case QMetaType::QJsonValue:
                        QCOMPARE(value.toJsonValue(), expectedValue.toJsonValue());
                        break;
                    default:
                        verify((const QMcpGadget *)value.constData(), data.value(name).toMap());
                        break;
                    }
                } else {
                    QCOMPARE(value, expectedValue);
                }
            }
        } else {
            switch (property.typeId()) {
            case QMetaType::QUrl:
                QCOMPARE(value.toUrl(), QUrl(expectedValue.toString()));
                break;
            case QMetaType::QJsonValue:
                QCOMPARE(value.toString(), expectedValue);
                break;
            default:
                QCOMPARE(value, expectedValue);
                break;
            }
        }
    }
}

} // namespace TestHelper

QT_END_NAMESPACE

#endif // TESTHELPER_H
