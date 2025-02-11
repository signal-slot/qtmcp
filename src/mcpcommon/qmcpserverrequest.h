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
public:
    QMcpServerRequest() : QMcpAnyOf(new Private) {}

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
