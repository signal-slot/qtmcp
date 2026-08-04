// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTCAPABILITIESELICITATION_H
#define QMCPCLIENTCAPABILITIESELICITATION_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpClientCapabilitiesElicitation
    \inmodule QtMcpCommon
    \brief Present if the client supports elicitation from the server.

    This capability has been introduced in the 2025-06-18 revision of the
    protocol.
*/
class Q_MCPCOMMON_EXPORT QMcpClientCapabilitiesElicitation : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpClientCapabilitiesElicitation() : QMcpGadget(new Private) {}

    QJsonObject additionalProperties() const {
        return d<Private>()->additionalProperties;
    }

    void setAdditionalProperties(const QJsonObject &props) {
        if (this->additionalProperties() == props) return;
        d<Private>()->additionalProperties = props;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject additionalProperties;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpClientCapabilitiesElicitation)

QT_END_NAMESPACE

#endif // QMCPCLIENTCAPABILITIESELICITATION_H
