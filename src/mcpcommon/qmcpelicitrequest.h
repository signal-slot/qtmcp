// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPELICITREQUEST_H
#define QMCPELICITREQUEST_H

#include <QtMcpCommon/qmcpelicitrequestparams.h>
#include <QtMcpCommon/qmcprequest.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpElicitRequest
    \inmodule QtMcpCommon
    \brief A request from the server to elicit additional information from the user via the client.

    This request has been introduced in the 2025-06-18 revision of the
    protocol.
*/
class Q_MCPCOMMON_EXPORT QMcpElicitRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpElicitRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpElicitRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "elicitation/create"_L1; }

    QMcpElicitRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpElicitRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpElicitRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpElicitRequest)

QT_END_NAMESPACE

#endif // QMCPELICITREQUEST_H
