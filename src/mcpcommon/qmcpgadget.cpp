// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpgadget.h"

#include <QtCore/qjsonarray.h>

QT_BEGIN_NAMESPACE

bool QMcpGadget::fromJsonObject(const QJsonObject &object)
{
    const auto mo = metaObject();

    for (int i = 0; i < mo->propertyCount(); i++) {
        const auto property = mo->property(i);
        if (property.isConstant())
            continue;
        const auto propertyType = property.typeName();
        const auto propertyName = QString::fromLatin1(property.name());
        if (!object.contains(propertyName)) {
            if (property.isRequired() && !property.isConstant())
                return false;
            continue;
        }

        const auto value = object.value(propertyName);
        if (value.isUndefined())
            continue;

        switch (value.type()) {
        case QJsonValue::Array: {
            const auto array = value.toArray();
            auto propertyValue = property.readOnGadget(this);
            // qDebug() << "Array" << propertyName << propertyValue << value.toArray();
            QByteArray typeName = property.typeName();
            if (typeName.startsWith('Q') && typeName.endsWith("List"_ba)) {
                typeName = typeName.chopped(4);
            } else if (typeName.startsWith("QList<"_ba) && typeName.endsWith('>')) {
                typeName = typeName.mid(6).chopped(1);
            } else {
                qFatal() << typeName;
            }
            const auto mt = QMetaType::fromName(typeName);
            if (!mt.isValid()) {
                qWarning() << "Unknown type" << typeName << propertyValue;
            } else {
                switch (mt.id()) {
                case QMetaType::Bool: {
                    QList<bool> *list = reinterpret_cast<QList<bool> *>(propertyValue.data());
                    for (const auto &v : array)
                        list->append(v.toBool());
                    break; }
                case QMetaType::Int: {
                    QList<int> *list = reinterpret_cast<QList<int> *>(propertyValue.data());
                    for (const auto &v : array)
                        list->append(v.toInt());
                    break; }
                case QMetaType::QByteArray: {
                    QList<QByteArray> *list = reinterpret_cast<QList<QByteArray> *>(propertyValue.data());
                    for (const auto &v : array) {
                        Q_ASSERT(v.isString());
                        list->append(v.toString().toLatin1());
                    }
                    break; }
                case QMetaType::QString: {
                    QList<QString> *list = reinterpret_cast<QList<QString> *>(propertyValue.data());
                    for (const auto &v : array) {
                        Q_ASSERT(v.isString());
                        list->append(v.toString());
                    }
                    break; }
                default: {
                    const auto mo = mt.metaObject();
                    if (mt.flags() & QMetaType::IsEnumeration) {
                        for (int i = 0; i < mo->enumeratorCount(); i++) {
                            const auto me = mo->enumerator(i);
                            // qDebug() << i << typeName << mo->className() << me.name();
                            if (typeName == QByteArray(mo->className()) + "::" + me.enumName()) {
                                QList<int> *list = reinterpret_cast<QList<int> *>(propertyValue.data());
                                for (const auto &str : array) {
                                    Q_ASSERT(str.isString());
                                    const auto string = str.toString();
                                    auto bytearray = string.toLatin1();
                                    const auto v = me.keyToValue(bytearray.constData());
                                    list->append(v);
                                }
                                break;
                            }
                        }
                    } else {
                        QList<QMcpGadget> *list = reinterpret_cast<QList<QMcpGadget> *>(propertyValue.data());
                        for (const auto &v : array) {
                            QMcpGadget base;
                            auto *sub = static_cast<QMcpGadget *>(mt.construct(&base));
                            if (!sub->fromJsonObject(v.toObject()))
                                return false;
                            list->append(*sub);
                        }
                    }
                    break; }
                }


                if (!property.writeOnGadget(this, propertyValue))
                    qFatal() << this;
            }
            break; }
        case QJsonValue::Object: {
            auto propertyValue = property.readOnGadget(this);
            if (propertyValue.canConvert<QMcpGadget>()) {
                auto *gadget = reinterpret_cast<QMcpGadget *>(propertyValue.data());
                if (!gadget->fromJsonObject(value.toObject()))
                    return false;
                if (!property.writeOnGadget(this, propertyValue))
                    qFatal() << gadget;
            } else {
                switch (property.typeId()) {
                case QMetaType::QJsonObject:
                    property.writeOnGadget(this, value.toObject());
                    break;
                case QMetaType::QJsonValue:
                    property.writeOnGadget(this, value);
                    break;
                default:
                    qFatal() << propertyType << "not supporeted for" << mo->className() <<  propertyName << propertyValue;
                    break;
                }
            }
            break; }
        default:
            if (!property.writeOnGadget(this, value.toVariant())) {
                qWarning() << propertyType << mo->className() << propertyName << value;
            }
            break;
        }
    }

    return true;
}

namespace {
QList<int> requiredOrModifiedPropertyIndices(const QMcpGadget *gadget)
{
    QList<int> ret;
    const auto mo = gadget->metaObject();
    auto other = static_cast<QMcpGadget *>(mo->metaType().create());
    for (int i = 0; i < mo->propertyCount(); ++i) {
        const auto property = mo->property(i);
        if (property.isRequired()) {
            ret.append(i);
        } else {
            const auto value1 = property.readOnGadget(gadget);
            const auto value2 = property.readOnGadget(other);
            if (value1 != value2)
                ret.append(i);
        }
    }
    delete other;
    return ret;
}
}

QJsonObject QMcpGadget::toJsonObject() const
{
    QJsonObject ret;
    const auto mo = metaObject();
    const auto indices = requiredOrModifiedPropertyIndices(this);
    for (int i : indices) {
        const auto mp = mo->property(i);
        const auto mt = mp.metaType();
        const auto name = QString::fromLatin1(mp.name());
        auto value = mp.readOnGadget(this);
        switch (mt.id()) {
        case QMetaType::Bool:
        case QMetaType::Int:
        case QMetaType::QString:
        case QMetaType::QJsonObject:
        case QMetaType::QJsonValue:
        case QMetaType::QVariant:
            break;
        case QMetaType::QByteArray:
            value = QString::fromUtf8(value.toByteArray());
            break;
        case QMetaType::QUrl:
            value = value.toUrl().toString();
            break;
        default:
            if (mt.flags() & QMetaType::IsEnumeration) {
                const auto mo = mt.metaObject();
                for (int i = 0; i < mo->enumeratorCount(); i++) {
                    const auto me = mo->enumerator(i);
                    if (mt.name() == QByteArray(mo->className()) + "::" + me.enumName()) {
                        const auto v = value.toInt();
                        value = QString::fromUtf8(me.valueToKey(v));
                        break;
                    }
                }
            } else if (value.canConvert<QMcpGadget>()) {
                const auto *gadget = reinterpret_cast<const QMcpGadget *>(value.constData());
                const auto object = gadget->toJsonObject();
                value = object.toVariantMap();
            } else if (value.canConvert<QVariantList>()) {
                QJsonArray array;
                const QVariantList list = value.toList();
                for (const auto &value : list) {
                    const QMetaType mt = value.metaType();
                    switch (mt.id()) {
                    case QMetaType::QByteArray:
                        array.append(QString::fromUtf8(value.toByteArray()));
                        break;
                    default:
                        if (mt.flags() & QMetaType::IsEnumeration) {
                            const auto mo = mt.metaObject();
                            for (int i = 0; i < mo->enumeratorCount(); i++) {
                                const auto me = mo->enumerator(i);
                                if (mt.name() == QByteArray(mo->className()) + "::" + me.enumName()) {
                                    const auto v = value.toInt();
                                    array.append(QString::fromUtf8(me.valueToKey(v)));
                                    break;
                                }
                            }
                        } else if (value.canConvert<QMcpGadget>()) {
                            const auto gadget = reinterpret_cast<const QMcpGadget *>(value.constData());
                            const auto object = gadget->toJsonObject();
                            array.append(object);
                        } else {
                            array.append(value.toJsonValue());
                        }
                        break;
                    }
                }
                value = array;
            }
        }
        ret.insert(name, value.toJsonValue());
    }
    return ret;
}

QT_END_NAMESPACE
