// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPREQUESTPARAMS_H
#define QMCPREQUESTPARAMS_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcprequestparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpRequestParams : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QMcpRequestParamsMeta _meta READ meta WRITE setMeta)
    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpRequestParams() : QMcpGadget(new Private) {}

    QMcpRequestParamsMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpRequestParamsMeta &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

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
        QMcpRequestParamsMeta _meta;
        QJsonObject additionalProperties;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPREQUESTPARAMS_H
