// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCOMPLETEREQUEST_H
#define QMCPCOMPLETEREQUEST_H

#include <QtMcpCommon/qmcpcompleterequestparams.h>
#include <QtMcpCommon/qmcprequest.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCompleteRequest
    \inmodule QtMcpCommon
    \brief A request from the client to the server, to ask for completion options.
*/
class Q_MCPCOMMON_EXPORT QMcpCompleteRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpCompleteRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpCompleteRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "completion/complete"_L1; }

    QMcpCompleteRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpCompleteRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpCompleteRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCompleteRequest)

QT_END_NAMESPACE

#endif // QMCPCOMPLETEREQUEST_H
