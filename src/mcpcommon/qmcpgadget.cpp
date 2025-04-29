// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpgadget.h"

#include <QtCore/qjsonarray.h>

QT_BEGIN_NAMESPACE

bool QMcpGadget::fromJsonObject(const QJsonObject &object, const QString &protocolVersion)
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
                        // Check if this is a list of pointers to QMcpGadget objects
                        if (typeName.endsWith('*')) {
                            // Handle QList<QMcpGadget *> case
                            QByteArray pointedTypeName = typeName.chopped(1).trimmed(); // Remove the * and any whitespace
                            auto metaType = QMetaType::fromName(pointedTypeName);

                            // Generic handling for lists of pointers to QMcpGadget subclasses
                            QList<QMcpGadget *> *list = reinterpret_cast<QList<QMcpGadget *> *>(propertyValue.data());

                            // Clear any existing items in the list first to avoid memory leaks
                            qDeleteAll(*list);
                            list->clear();

                            for (const auto &v : array) {
                                if (!v.isObject())
                                    continue;

                                // Create a new instance
                                QMcpGadget *gadget = nullptr;

                                if (metaType.isValid()) {
                                    // Create using the meta-type if available
                                    void *instance = metaType.create();
                                    if (instance) {
                                        gadget = static_cast<QMcpGadget *>(instance);
                                    }
                                }

                                // If the meta-type approach failed, fall back to creating a generic instance
                                if (!gadget) {
                                    // As fallback, create a generic QMcpGadget instance
                                    // since we can't know the exact type
                                    gadget = new QMcpGadget();
                                }

                                // Populate it from JSON
                                if (!gadget->fromJsonObject(v.toObject(), protocolVersion)) {
                                    delete gadget;
                                    return false;
                                }

                                // Add it to the list
                                list->append(gadget);
                            }
                        } else {
                            // Original code for handling QList<QMcpGadget>
                            QList<QMcpGadget> *list = reinterpret_cast<QList<QMcpGadget> *>(propertyValue.data());
                            for (const auto &v : array) {
                                QMcpGadget base;
                                auto *sub = static_cast<QMcpGadget *>(mt.construct(&base));
                                if (!sub->fromJsonObject(v.toObject(), protocolVersion)) {
                                    delete sub;
                                    return false;
                                }
                                list->append(*sub);
                            }
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
                if (!gadget->fromJsonObject(value.toObject(), protocolVersion))
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

QJsonObject QMcpGadget::toJsonObject(const QString &protocolVersion) const
{
    QJsonObject ret;
    const auto mo = metaObject();
    const auto indices = requiredOrModifiedPropertyIndices(this);
    for (int i : indices) {
        const auto mp = mo->property(i);
        const auto mt = mp.metaType();
        const auto name = QString::fromLatin1(mp.name());
        auto value = mp.readOnGadget(this);

        // Handle QList types first to ensure empty lists are properly serialized
        if (QString::fromUtf8(mt.name()).startsWith(QStringLiteral("QList<")) ||
            QString::fromUtf8(mt.name()).endsWith(QStringLiteral("List"))) {
            // Always convert to a JSON array even if the list is empty
            QJsonArray array;
            if (value.canConvert<QVariantList>()) {
                const QVariantList list = value.toList();
                for (const auto &item : list) {
                    const QMetaType itemType = item.metaType();
                    switch (itemType.id()) {
                    case QMetaType::QByteArray:
                        array.append(QString::fromUtf8(item.toByteArray()));
                        break;
                    default:
                        if (itemType.flags() & QMetaType::IsEnumeration) {
                            const auto mo = itemType.metaObject();
                            for (int i = 0; i < mo->enumeratorCount(); i++) {
                                const auto me = mo->enumerator(i);
                                if (itemType.name() == QByteArray(mo->className()) + "::" + me.enumName()) {
                                    const auto v = item.toInt();
                                    array.append(QString::fromUtf8(me.valueToKey(v)));
                                    break;
                                }
                            }
                        } else if (item.canConvert<QMcpGadget>()) {
                            const auto gadget = reinterpret_cast<const QMcpGadget *>(item.constData());
                            const auto object = gadget->toJsonObject(protocolVersion);
                            array.append(object);
                        } else if (item.canConvert<QMcpGadget *>()) {
                            const auto gadget = item.value<QMcpGadget *>();
                            const auto object = gadget->toJsonObject(protocolVersion);
                            array.append(object);
                        } else {
                            array.append(item.toJsonValue());
                        }
                        break;
                    }
                }
            }
            ret.insert(name, array);
            continue;
        }

        // Handle other types
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
                const auto object = gadget->toJsonObject(protocolVersion);
                value = object.toVariantMap();
            }
        }
        ret.insert(name, value.toJsonValue());
    }
    return ret;
}

QT_END_NAMESPACE
