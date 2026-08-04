// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpanyof.h"

QT_BEGIN_NAMESPACE

bool QMcpAnyOf::fromJsonObject(const QJsonObject &object, QtMcp::ProtocolVersion protocolVersion)
{
    const auto mo = metaObject();
    int propertyIndex = d<Private>()->findPropertyIndex(object);
    if (propertyIndex < 0) {
        auto typeMatches = [&object](const QMcpGadget *gadget) -> bool {
            auto keys = object.keys();
            const auto mo = gadget->metaObject();
            for (int j = 0; j < mo->propertyCount(); j++) {
                const auto property = mo->property(j);
                const auto propertyName = QString::fromLatin1(property.name());
                const auto isConstant = property.isConstant();
                const auto isRequired = property.isRequired();
                if (keys.contains(propertyName)) {
                    keys.removeOne(propertyName);
                } else if (isRequired) {
                    // object must contain required property
                    return false;
                } else if (isConstant) {
                    // const property must match
                    const auto propertyValue = property.readOnGadget(gadget);
                    if (object.value(propertyName).toVariant() != propertyValue) {
                        qWarning() << mo->className() << propertyName << object.value(propertyName) << propertyValue;
                        return false;
                    }
                }
            }
            // if object contains unknown property, return false
            return keys.isEmpty();
        };

        QList<int> matched;
        const auto mo2 = &staticMetaObject;
        for (int i = mo2->propertyOffset() + mo2->propertyCount(); i < mo->propertyCount(); i++) {
            const auto property = mo->property(i);
            auto propertyValue = property.readOnGadget(this);
            if (propertyValue.canConvert<QMcpGadget>()) {
                auto *gadget = reinterpret_cast<QMcpGadget *>(propertyValue.data());
                if (!typeMatches(gadget))
                    continue;
                matched.append(i);
            } else {
                qFatal();
            }
        }

        if (matched.count() != 1) {
            QStringList propertyNames;
            for (const auto index : matched) {
                const auto property = mo->property(index);
                propertyNames.append(QString::fromLatin1(property.name()));
            }
            qWarning() << "More than one property candidates found" << propertyNames;
            qWarning() << "Please implement findPropertyIndex() to specify the property";
            return false;
        }
        propertyIndex = matched.first();
    }

    const auto property = mo->property(propertyIndex);
    // The refType has to be set before the property is written: the setters
    // only set it when the value actually changes, so a variant that happens to
    // equal its default value would otherwise leave the union unset.
    setRefType(property.name());
    auto propertyValue = property.readOnGadget(this);
    if (propertyValue.canConvert<QMcpGadget>()) {
        auto *gadget = reinterpret_cast<QMcpGadget *>(propertyValue.data());
        // Let the variant parse the object itself instead of duplicating the
        // property handling here. QMcpGadget::fromJsonObject() also covers
        // arrays, enums and version gated properties, and it keeps this in sync
        // with toJsonObject(), which delegates to the variant as well.
        if (!gadget->fromJsonObject(object, protocolVersion))
            return false;
        property.writeOnGadget(this, propertyValue);
    }
    return true;
}

QJsonObject QMcpAnyOf::toJsonObject(QtMcp::ProtocolVersion protocolVersion) const
{
    const auto mo = metaObject();
    for (int i = 0; i < mo->propertyCount(); i++) {
        const auto mp = mo->property(i);
        if (refType() != mp.name())
            continue;
        auto value = mp.readOnGadget(this);
        if (value.canConvert<QMcpGadget>()) {
            const auto *gadget = reinterpret_cast<const QMcpGadget *>(value.constData());
            return gadget->toJsonObject(protocolVersion);
        } else {
            qFatal();
        }
    }
    qWarning() << refType() << "not found";
    return {};
}

QT_END_NAMESPACE
