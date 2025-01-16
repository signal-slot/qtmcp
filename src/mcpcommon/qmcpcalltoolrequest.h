// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCALLTOOLREQUEST_H
#define QMCPCALLTOOLREQUEST_H

#include <QtMcpCommon/qmcpcalltoolrequestparams.h>
#include <QtMcpCommon/qmcprequest.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCallToolRequest
    \inmodule QtMcpCommon
    \brief Used by the client to invoke a tool provided by the server.
*/
class Q_MCPCOMMON_EXPORT QMcpCallToolRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpCallToolRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpCallToolRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "tools/call"_L1; }

    QMcpCallToolRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpCallToolRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpCallToolRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCALLTOOLREQUEST_H
