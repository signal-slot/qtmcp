// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPDISCOVERREQUEST_H
#define QMCPDISCOVERREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcprequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpDiscoverRequest
    \inmodule QtMcpCommon
    \brief A request from the client asking the server to advertise its supported
    protocol versions, capabilities, and other metadata.

    Servers must implement \c server/discover. Clients may call it but are not
    required to, since version negotiation can also happen inline via the
    per-request \c _meta.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpDiscoverRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpRequestParams params READ params WRITE setParams)

public:
    QMcpDiscoverRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "server/discover"_L1; }

    QMcpRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpDiscoverRequest)

QT_END_NAMESPACE

#endif // QMCPDISCOVERREQUEST_H
