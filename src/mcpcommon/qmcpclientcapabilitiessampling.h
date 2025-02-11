// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTCAPABILITIESSAMPLING_H
#define QMCPCLIENTCAPABILITIESSAMPLING_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpClientCapabilitiesSampling
    \inmodule QtMcpCommon
    \brief Present if the client supports sampling from an LLM.
*/
class Q_MCPCOMMON_EXPORT QMcpClientCapabilitiesSampling : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpClientCapabilitiesSampling() : QMcpGadget(new Private) {}

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
    public:
        QJsonObject additionalProperties;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpClientCapabilitiesSampling)

QT_END_NAMESPACE

#endif // QMCPCLIENTCAPABILITIESSAMPLING_H
