#ifndef QTMCPNAMESPACE_H
#define QTMCPNAMESPACE_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtCore/QString>
#include <QtCore/QMetaType>

QT_BEGIN_NAMESPACE

namespace QtMcp {

// Protocol version enum
// Values are date-ordered (YYYYMMDD) so relational comparison between
// versions is meaningful, e.g. version >= ProtocolVersion::v2025_06_18.
enum class ProtocolVersion {
    v2024_11_05 = 20241105,
    v2025_03_26 = 20250326,
    v2025_06_18 = 20250618,
    v2025_11_25 = 20251125,
    v2026_07_28 = 20260728,

    Latest = v2026_07_28
};

// Convert protocol version enum to string
inline QString protocolVersionToString(ProtocolVersion version) {
    switch (version) {
    case ProtocolVersion::v2024_11_05:
        return QStringLiteral("2024-11-05");
    case ProtocolVersion::v2025_03_26:
        return QStringLiteral("2025-03-26");
    case ProtocolVersion::v2025_06_18:
        return QStringLiteral("2025-06-18");
    case ProtocolVersion::v2025_11_25:
        return QStringLiteral("2025-11-25");
    case ProtocolVersion::v2026_07_28:
    default:
        return QStringLiteral("2026-07-28");
    }
}

// Convert protocol version string to enum
inline ProtocolVersion stringToProtocolVersion(const QString &versionStr) {
    if (versionStr == QLatin1String("2024-11-05"))
        return ProtocolVersion::v2024_11_05;
    else if (versionStr == QLatin1String("2025-03-26"))
        return ProtocolVersion::v2025_03_26;
    else if (versionStr == QLatin1String("2025-06-18"))
        return ProtocolVersion::v2025_06_18;
    else if (versionStr == QLatin1String("2025-11-25"))
        return ProtocolVersion::v2025_11_25;
    else if (versionStr == QLatin1String("2026-07-28"))
        return ProtocolVersion::v2026_07_28;
    else
        return ProtocolVersion::Latest; // Default to latest for unknown values
}

} // namespace QtMcp

// Register the enum with the Qt meta-object system
Q_DECLARE_METATYPE(QtMcp::ProtocolVersion)

QT_END_NAMESPACE

#endif // QTMCPNAMESPACE_H
