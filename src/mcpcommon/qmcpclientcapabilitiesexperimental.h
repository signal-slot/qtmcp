// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTCAPABILITIESEXPERIMENTAL_H
#define QMCPCLIENTCAPABILITIESEXPERIMENTAL_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpClientCapabilitiesExperimental
    \inmodule QtMcpCommon
    \brief Experimental, non-standard capabilities that the client supports.
*/
class Q_MCPCOMMON_EXPORT QMcpClientCapabilitiesExperimental : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpClientCapabilitiesExperimental() : QMcpGadget(new Private) {}

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

QT_END_NAMESPACE

#endif // QMCPCLIENTCAPABILITIESEXPERIMENTAL_H
