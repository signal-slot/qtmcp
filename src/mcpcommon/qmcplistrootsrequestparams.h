// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTROOTSREQUESTPARAMS_H
#define QMCPLISTROOTSREQUESTPARAMS_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcplistrootsrequestparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListRootsRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpListRootsRequestParams : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QMcpListRootsRequestParamsMeta _meta READ _meta WRITE setMeta)

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpListRootsRequestParams() : QMcpGadget(new Private) {}

    QMcpListRootsRequestParamsMeta _meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpListRootsRequestParamsMeta &meta) {
        if (_meta() == meta) return;
        d<Private>()->_meta = meta;
    }

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
        QMcpListRootsRequestParamsMeta _meta;
        QJsonObject additionalProperties;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListRootsRequestParams)

QT_END_NAMESPACE

#endif // QMCPLISTROOTSREQUESTPARAMS_H
