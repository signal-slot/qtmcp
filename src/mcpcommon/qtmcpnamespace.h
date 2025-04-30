#ifndef QTMCPNAMESPACE_H
#define QTMCPNAMESPACE_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtCore/QString>
#include <QtCore/QMetaType>

QT_BEGIN_NAMESPACE

namespace QtMcp {

// Protocol version enum
enum class ProtocolVersion {
    v2024_11_05 = 20241105,
    v2025_03_26 = 20250326,

    Latest = v2025_03_26
};

// Convert protocol version enum to string
inline QString protocolVersionToString(ProtocolVersion version) {
    switch (version) {
    case ProtocolVersion::v2024_11_05:
        return QStringLiteral("2024-11-05");
    case ProtocolVersion::v2025_03_26:
    default:
        return QStringLiteral("2025-03-26");
    }
}

// Convert protocol version string to enum
inline ProtocolVersion stringToProtocolVersion(const QString &versionStr) {
    if (versionStr == QLatin1String("2024-11-05"))
        return ProtocolVersion::v2024_11_05;
    else if (versionStr == QLatin1String("2025-03-26"))
        return ProtocolVersion::v2025_03_26;
    else
        return ProtocolVersion::Latest; // Default to latest for unknown values
}

} // namespace QtMcp

// Register the enum with the Qt meta-object system
Q_DECLARE_METATYPE(QtMcp::ProtocolVersion)

QT_END_NAMESPACE

#endif // QTMCPNAMESPACE_H
