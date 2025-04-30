// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPINITIALIZEREQUESTPARAMS_H
#define QMCPINITIALIZEREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpclientcapabilities.h>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpimplementation.h>
#include <QtMcpCommon/qtmcpnamespace.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpInitializeRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpInitializeRequestParams : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QMcpClientCapabilities capabilities READ capabilities WRITE setCapabilities REQUIRED)
    Q_PROPERTY(QMcpImplementation clientInfo READ clientInfo WRITE setClientInfo REQUIRED)

    /*!
        \property QMcpInitializeRequestParams::protocolVersion
        \brief The latest version of the Model Context Protocol that the client supports. The client MAY decide to support older versions as well.
    */
    Q_PROPERTY(QtMcp::ProtocolVersion protocolVersion READ protocolVersion WRITE setProtocolVersion REQUIRED)

public:
    QMcpInitializeRequestParams() : QMcpGadget(new Private) {}

    QMcpClientCapabilities capabilities() const {
        return d<Private>()->capabilities;
    }

    void setCapabilities(const QMcpClientCapabilities &capabilities) {
        if (this->capabilities() == capabilities) return;
        d<Private>()->capabilities = capabilities;
    }

    QMcpImplementation clientInfo() const {
        return d<Private>()->clientInfo;
    }

    void setClientInfo(const QMcpImplementation &clientInfo) {
        if (this->clientInfo() == clientInfo) return;
        d<Private>()->clientInfo = clientInfo;
    }

    QtMcp::ProtocolVersion protocolVersion() const {
        return d<Private>()->protocolVersion;
    }

    void setProtocolVersion(QtMcp::ProtocolVersion version) {
        if (this->protocolVersion() == version) return;
        d<Private>()->protocolVersion = version;
    }
    
    // For backward compatibility
    void setProtocolVersion(const QString &versionStr) {
        setProtocolVersion(QtMcp::stringToProtocolVersion(versionStr));
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpClientCapabilities capabilities;
        QMcpImplementation clientInfo;
        QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpInitializeRequestParams)

QT_END_NAMESPACE

#endif // QMCPINITIALIZEREQUESTPARAMS_H
