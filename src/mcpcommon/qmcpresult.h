// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESULT_H
#define QMCPRESULT_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpresultmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResult
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpResult : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpResult::_meta
        \brief This result property is reserved by the protocol to allow clients and servers to attach additional metadata to their responses.
    */
    Q_PROPERTY(QMcpResultMeta _meta READ meta WRITE setMeta)
    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

public:
    QMcpResult() : QMcpGadget(new Private) {}
protected:
    QMcpResult(Private *d) : QMcpGadget(d) {}
public:

    QMcpResultMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpResultMeta &meta) {
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

protected:
    struct Private : public QMcpGadget::Private {
        QMcpResultMeta _meta;
        QJsonObject additionalProperties;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPRESULT_H
