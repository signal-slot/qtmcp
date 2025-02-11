// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPINGREQUEST_H
#define QMCPPINGREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcppingrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPingRequest
    \inmodule QtMcpCommon
    \brief A ping, issued by either the server or the client, to check that the other party is still alive. The receiver must promptly respond, or else may be disconnected.
*/
class Q_MCPCOMMON_EXPORT QMcpPingRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpPingRequestParams params READ params WRITE setParams)

public:
    QMcpPingRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "ping"_L1; }

    QMcpPingRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpPingRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpPingRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPingRequest)

QT_END_NAMESPACE

#endif // QMCPPINGREQUEST_H
