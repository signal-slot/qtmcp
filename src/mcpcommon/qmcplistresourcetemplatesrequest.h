// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTRESOURCETEMPLATESREQUEST_H
#define QMCPLISTRESOURCETEMPLATESREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcplistresourcetemplatesrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListResourceTemplatesRequest
    \inmodule QtMcpCommon
    \brief Sent from the client to request a list of resource templates the server has.
*/
class Q_MCPCOMMON_EXPORT QMcpListResourceTemplatesRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpListResourceTemplatesRequestParams params READ params WRITE setParams)

public:
    QMcpListResourceTemplatesRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "resources/templates/list"_L1; }

    QMcpListResourceTemplatesRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpListResourceTemplatesRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpListResourceTemplatesRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPLISTRESOURCETEMPLATESREQUEST_H
