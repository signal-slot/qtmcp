// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESULTMETA_H
#define QMCPRESULTMETA_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResultMeta
    \inmodule QtMcpCommon
    \brief This result property is reserved by the protocol to allow clients and servers to attach additional metadata to their responses.
*/
class Q_MCPCOMMON_EXPORT QMcpResultMeta : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpResultMeta() : QMcpGadget(new Private) {}
protected:
    QMcpResultMeta(Private *d) : QMcpGadget(d) {}
public:

    QJsonObject additionalProperties() const {
        return d<Private>()->additionalProperties;
    }

    void setAdditionalProperties(const QJsonObject& additionalProperties) {
        if (this->additionalProperties() == additionalProperties) return;
        d<Private>()->additionalProperties = additionalProperties;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpGadget::Private {
        QJsonObject additionalProperties;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPRESULTMETA_H
