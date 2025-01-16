// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTRESULT_H
#define QMCPCLIENTRESULT_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcpcreatemessageresult.h>
#include <QtMcpCommon/qmcplistrootsresult.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpClientResult : public QMcpAnyOf
{
    Q_GADGET
    Q_PROPERTY(QMcpResult result READ result WRITE setResult)
    Q_PROPERTY(QMcpCreateMessageResult createMessageResult READ createMessageResult WRITE setCreateMessageResult)
    Q_PROPERTY(QMcpListRootsResult listRootsResult READ listRootsResult WRITE setListRootsResult)
public:
    QMcpClientResult() : QMcpAnyOf(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QMcpResult result() const {
        return d<Private>()->result;
    }

    void setResult(const QMcpResult &result) {
        if (this->result() == result) return;
        setRefType("result"_ba);
        d<Private>()->result = result;
    }

    QMcpCreateMessageResult createMessageResult() const {
        return d<Private>()->createMessageResult;
    }

    void setCreateMessageResult(const QMcpCreateMessageResult &createMessageResult) {
        if (this->createMessageResult() == createMessageResult) return;
        setRefType("createMessageResult"_ba);
        d<Private>()->createMessageResult = createMessageResult;
    }

    QMcpListRootsResult listRootsResult() const {
        return d<Private>()->listRootsResult;
    }

    void setListRootsResult(const QMcpListRootsResult &listRootsResult) {
        if (this->listRootsResult() == listRootsResult) return;
        setRefType("listRootsResult"_ba);
        d<Private>()->listRootsResult = listRootsResult;
    }

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpResult result;
        QMcpCreateMessageResult createMessageResult;
        QMcpListRootsResult listRootsResult;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCLIENTRESULT_H
