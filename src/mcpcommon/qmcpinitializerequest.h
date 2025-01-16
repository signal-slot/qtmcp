// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPINITIALIZEREQUEST_H
#define QMCPINITIALIZEREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpinitializerequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpInitializeRequest
    \inmodule QtMcpCommon
    \brief This request is sent from the client to the server when it first connects, asking it to begin initialization.
*/
class Q_MCPCOMMON_EXPORT QMcpInitializeRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpInitializeRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpInitializeRequest() : QMcpRequest(new Private) {}

    QString method() const final {
        return "initialize"_L1;
    }

    QMcpInitializeRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpInitializeRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpInitializeRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPINITIALIZEREQUEST_H
