// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSETLEVELREQUEST_H
#define QMCPSETLEVELREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpsetlevelrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSetLevelRequest
    \inmodule QtMcpCommon
    \brief A request from the client to the server, to enable or adjust logging.
*/
class Q_MCPCOMMON_EXPORT QMcpSetLevelRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpSetLevelRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpSetLevelRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "logging/setLevel"_L1; }

    QMcpSetLevelRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpSetLevelRequestParams& params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpSetLevelRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPSETLEVELREQUEST_H
