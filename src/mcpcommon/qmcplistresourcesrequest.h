// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTRESOURCESREQUEST_H
#define QMCPLISTRESOURCESREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcplistresourcesrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListResourcesRequest
    \inmodule QtMcpCommon
    \brief Sent from the client to request a list of resources the server has.
*/
class Q_MCPCOMMON_EXPORT QMcpListResourcesRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpListResourcesRequestParams params READ params WRITE setParams)

public:
    QMcpListResourcesRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "resources/list"_L1; }

    QMcpListResourcesRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpListResourcesRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpListResourcesRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPLISTRESOURCESREQUEST_H
