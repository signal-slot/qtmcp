// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpannotated.h"

QT_BEGIN_NAMESPACE

QJsonObject QMcpAnnotated::toJsonObject(const QString &protocolVersion) const
{
    // Get the base object representation
    QJsonObject obj = QMcpGadget::toJsonObject(protocolVersion);

    // Special case: 2024-11-05 has no annotations
    if (protocolVersion == "2024-11-05"_L1) {
        // Explicitly remove annotations key even if it exists
        if (obj.contains("annotations"_L1))
            obj.remove("annotations"_L1);
        return obj;
    }

    // For 2025-03-26 and all future versions:
    // Include annotations if they're not empty
    QMcpAnnotations anns = annotations();
    if (!anns.audience().isEmpty() || anns.priority() != 0.0) {
        obj["annotations"_L1] = anns.toJsonObject(protocolVersion);
    }

    return obj;
}

bool QMcpAnnotated::fromJsonObject(const QJsonObject &object, const QString &protocolVersion)
{
    // Parse the base object first
    if (!QMcpGadget::fromJsonObject(object, protocolVersion))
        return false;

    // 2024-11-05: Always reset annotations to empty and return success
    if (protocolVersion == "2024-11-05"_L1) {
        setAnnotations(QMcpAnnotations());
        return true;
    }

    // For 2025-03-26 and later versions:
    if (object.contains("annotations"_L1)) {
        QMcpAnnotations anns;
        if (anns.fromJsonObject(object.value("annotations"_L1).toObject())) {
            setAnnotations(anns);
        }
    }

    return true;
}

QT_END_NAMESPACE
