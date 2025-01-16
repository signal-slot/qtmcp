// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOLLISTCHANGEDNOTIFICATIONPARAMSMETA_H
#define QMCPTOOLLISTCHANGEDNOTIFICATIONPARAMSMETA_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtCore/qjsonobject.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpToolListChangedNotificationParamsMeta
    \inmodule QtMcpCommon
    \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
*/
class Q_MCPCOMMON_EXPORT QMcpToolListChangedNotificationParamsMeta : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpToolListChangedNotificationParamsMeta() : QMcpGadget(new Private) {}

    QJsonObject additionalProperties() const {
        return d<Private>()->additionalProperties;
    }

    void setAdditionalProperties(const QJsonObject &properties) {
        if (additionalProperties() == properties) return;
        d<Private>()->additionalProperties = properties;
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

#endif // QMCPTOOLLISTCHANGEDNOTIFICATIONPARAMSMETA_H
