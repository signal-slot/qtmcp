// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpannotated.h"

QT_BEGIN_NAMESPACE

QJsonObject QMcpAnnotated::toJsonObject(const QString &protocolVersion) const
{
    // Get the base object representation
    QJsonObject obj = QMcpGadget::toJsonObject(protocolVersion);
    
    // Handle annotations based on protocol version
    if (protocolVersion == "2025-03-26"_L1) {
        // Only include annotations if they're not empty for 2025-03-26
        QMcpAnnotations anns = annotations();
        if (!anns.audience().isEmpty() || anns.priority() != 0) {
            obj["annotations"_L1] = anns.toJsonObject(protocolVersion);
        }
    }
    else if (protocolVersion == "2024-11-05"_L1) {
        // Explicitly omit annotations for 2024-11-05 as they weren't supported
        // No action needed as we don't add annotations
    }
    else {
        // For unknown versions, default to the latest behavior
        QMcpAnnotations anns = annotations();
        if (!anns.audience().isEmpty() || anns.priority() != 0) {
            obj["annotations"_L1] = anns.toJsonObject(protocolVersion);
        }
    }
    
    return obj;
}

bool QMcpAnnotated::fromJsonObject(const QJsonObject &object, const QString &protocolVersion)
{
    // Parse the base object first
    if (!QMcpGadget::fromJsonObject(object, protocolVersion))
        return false;
    
    // Handle annotations based on protocol version
    if (protocolVersion == "2025-03-26"_L1) {
        // Only process annotations if they're present in the JSON for 2025-03-26
        if (object.contains("annotations"_L1)) {
            QMcpAnnotations anns;
            if (!anns.fromJsonObject(object["annotations"_L1].toObject(), protocolVersion))
                return false;
            setAnnotations(anns);
        }
    }
    else if (protocolVersion == "2024-11-05"_L1) {
        // Explicitly ignore annotations for 2024-11-05 as they weren't supported
        // No action needed as we don't process annotations
    }
    else {
        // For unknown versions, default to the latest behavior
        if (object.contains("annotations"_L1)) {
            QMcpAnnotations anns;
            if (!anns.fromJsonObject(object["annotations"_L1].toObject(), protocolVersion))
                return false;
            setAnnotations(anns);
        }
    }
    
    return true;
}

QT_END_NAMESPACE
