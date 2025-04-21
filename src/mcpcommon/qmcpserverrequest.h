// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERREQUEST_H
#define QMCPSERVERREQUEST_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcppingrequest.h>
#include <QtMcpCommon/qmcpcreatemessagerequest.h>
#include <QtMcpCommon/qmcplistrootsrequest.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpServerRequest : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpPingRequest pingRequest READ pingRequest WRITE setPingRequest)
    Q_PROPERTY(QMcpCreateMessageRequest createMessageRequest READ createMessageRequest WRITE setCreateMessageRequest)
    Q_PROPERTY(QMcpListRootsRequest listRootsRequest READ listRootsRequest WRITE setListRootsRequest)

public:
    QMcpServerRequest() : QMcpAnyOf(new Private) {}

    QMcpPingRequest pingRequest() const {
        return d<Private>()->pingRequest;
    }

    void setPingRequest(const QMcpPingRequest &request) {
        if (this->pingRequest() == request) return;
        setRefType("pingRequest");
        d<Private>()->pingRequest = request;
    }

    QMcpCreateMessageRequest createMessageRequest() const {
        return d<Private>()->createMessageRequest;
    }

    void setCreateMessageRequest(const QMcpCreateMessageRequest &request) {
        if (this->createMessageRequest() == request) return;
        setRefType("createMessageRequest");
        d<Private>()->createMessageRequest = request;
    }

    QMcpListRootsRequest listRootsRequest() const {
        return d<Private>()->listRootsRequest;
    }

    void setListRootsRequest(const QMcpListRootsRequest &request) {
        if (this->listRootsRequest() == request) return;
        setRefType("listRootsRequest");
        d<Private>()->listRootsRequest = request;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpPingRequest pingRequest;
        QMcpCreateMessageRequest createMessageRequest;
        QMcpListRootsRequest listRootsRequest;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpServerRequest)

QT_END_NAMESPACE

#endif // QMCPSERVERREQUEST_H
