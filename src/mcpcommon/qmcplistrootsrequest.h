// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTROOTSREQUEST_H
#define QMCPLISTROOTSREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcplistrootsrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListRootsRequest
    \inmodule QtMcpCommon
    \brief Sent from the server to request a list of root URIs from the client. Roots allow
    servers to ask for specific directories or files to operate on. A common example
    for roots is providing a set of repositories or directories a server should operate
    on.
    
    This request is typically used when the server needs to understand the file system
    structure or access specific locations that the client has permission to read from.
*/
class Q_MCPCOMMON_EXPORT QMcpListRootsRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpListRootsRequestParams params READ params WRITE setParams)

public:
    QMcpListRootsRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "roots/list"_L1; }

    QMcpListRootsRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpListRootsRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpListRootsRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPLISTROOTSREQUEST_H
