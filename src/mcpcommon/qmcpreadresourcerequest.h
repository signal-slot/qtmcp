// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPREADRESOURCEREQUEST_H
#define QMCPREADRESOURCEREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpreadresourcerequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpReadResourceRequest
    \inmodule QtMcpCommon
    \brief Sent from the client to the server, to read a specific resource URI.
*/
class Q_MCPCOMMON_EXPORT QMcpReadResourceRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpReadResourceRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpReadResourceRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "resources/read"_L1; }

    QMcpReadResourceRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpReadResourceRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpReadResourceRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpReadResourceRequest)

QT_END_NAMESPACE

#endif // QMCPREADRESOURCEREQUEST_H
