// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPINITIALIZEREQUESTPARAMS_H
#define QMCPINITIALIZEREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpclientcapabilities.h>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpimplementation.h>

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
    Q_PROPERTY(QString protocolVersion READ protocolVersion WRITE setProtocolVersion REQUIRED)

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

    QString protocolVersion() const {
        return d<Private>()->protocolVersion;
    }

    void setProtocolVersion(const QString &version) {
        if (this->protocolVersion() == version) return;
        d<Private>()->protocolVersion = version;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpClientCapabilities capabilities;
        QMcpImplementation clientInfo;
        QString protocolVersion;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPINITIALIZEREQUESTPARAMS_H
