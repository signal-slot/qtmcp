// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCELISTCHANGEDNOTIFICATIONPARAMSMETA_H
#define QMCPRESOURCELISTCHANGEDNOTIFICATIONPARAMSMETA_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceListChangedNotificationParamsMeta
    \inmodule QtMcpCommon
    \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
*/
class Q_MCPCOMMON_EXPORT QMcpResourceListChangedNotificationParamsMeta : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpResourceListChangedNotificationParamsMeta() : QMcpGadget(new Private) {}

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

Q_DECLARE_SHARED(QMcpResourceListChangedNotificationParamsMeta)

QT_END_NAMESPACE

#endif // QMCPRESOURCELISTCHANGEDNOTIFICATIONPARAMSMETA_H
