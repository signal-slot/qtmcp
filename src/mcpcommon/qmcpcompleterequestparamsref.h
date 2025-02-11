// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCOMPLETEREQUESTPARAMSREF_H
#define QMCPCOMPLETEREQUESTPARAMSREF_H

#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcppromptreference.h>
#include <QtMcpCommon/qmcpresourcereference.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpCompleteRequestParamsRef : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpPromptReference promptReference READ promptReference WRITE setPromptReference)
    Q_PROPERTY(QMcpResourceReference resourceReference READ resourceReference WRITE setResourceReference)
public:
    QMcpCompleteRequestParamsRef() : QMcpAnyOf(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QMcpPromptReference promptReference() const {
        return d<Private>()->promptReference;
    }

    void setPromptReference(const QMcpPromptReference &promptReference) {
        if (this->promptReference() == promptReference) return;
        setRefType("promptReference"_ba);
        d<Private>()->promptReference = promptReference;
    }

    QMcpResourceReference resourceReference() const {
        return d<Private>()->resourceReference;
    }

    void setResourceReference(const QMcpResourceReference &resourceReference) {
        if (this->resourceReference() == resourceReference) return;
        setRefType("resourceReference"_ba);
        d<Private>()->resourceReference = resourceReference;
    }
private:
    struct Private : public QMcpAnyOf::Private {
        QMcpPromptReference promptReference;
        QMcpResourceReference resourceReference;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCompleteRequestParamsRef)

QT_END_NAMESPACE

#endif // QMCPCOMPLETEREQUESTPARAMSREF_H
