// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCREATEMESSAGEREQUESTPARAMSMETADATA_H
#define QMCPCREATEMESSAGEREQUESTPARAMSMETADATA_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCreateMessageRequestParamsMetadata
    \inmodule QtMcpCommon
    \brief Optional metadata to pass through to the LLM provider. The format of this metadata is provider-specific.
*/
class Q_MCPCOMMON_EXPORT QMcpCreateMessageRequestParamsMetadata : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpCreateMessageRequestParamsMetadata() : QMcpGadget(new Private) {}

    QJsonObject additionalProperties() const { return d<Private>()->additionalProperties; }
    void setAdditionalProperties(const QJsonObject &value) {
        if (additionalProperties() == value) return;
        d<Private>()->additionalProperties = value;
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

#endif // QMCPCREATEMESSAGEREQUESTPARAMSMETADATA_H
