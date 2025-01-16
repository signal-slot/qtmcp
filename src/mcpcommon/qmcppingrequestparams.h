// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPINGREQUESTPARAMS_H
#define QMCPPINGREQUESTPARAMS_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcppingrequestparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPingRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpPingRequestParams : public QMcpGadget
{
    Q_GADGET

#if 0
    Q_PROPERTY(QMcpPingRequestParamsMeta _meta READ meta WRITE setMeta)
    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)
#endif

public:
    QMcpPingRequestParams() : QMcpGadget(new Private) {}

#if 0
    QMcpPingRequestParamsMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpPingRequestParamsMeta &meta) {
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
#endif

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
#if 0
        QMcpPingRequestParamsMeta _meta;
        QJsonObject additionalProperties;
#endif
        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPPINGREQUESTPARAMS_H
