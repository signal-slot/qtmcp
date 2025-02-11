// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTTOOLSREQUEST_H
#define QMCPLISTTOOLSREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcplisttoolsrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListToolsRequest
    \inmodule QtMcpCommon
    \brief Sent from the client to request a list of tools the server has.
*/
class Q_MCPCOMMON_EXPORT QMcpListToolsRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpListToolsRequestParams params READ params WRITE setParams)

public:
    QMcpListToolsRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "tools/list"_L1; }

    QMcpListToolsRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpListToolsRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpListToolsRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListToolsRequest)

QT_END_NAMESPACE

#endif // QMCPLISTTOOLSREQUEST_H
