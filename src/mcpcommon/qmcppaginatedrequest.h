// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPAGINATEDREQUEST_H
#define QMCPPAGINATEDREQUEST_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcppaginatedrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPaginatedRequest
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpPaginatedRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QString method READ method WRITE setMethod REQUIRED)
    Q_PROPERTY(QMcpPaginatedRequestParams params READ params WRITE setParams)

public:
    QMcpPaginatedRequest() : QMcpRequest(new Private) {}

    QString method() const final {
        return d<Private>()->method;
    }

    void setMethod(const QString &method) {
        if (this->method() == method) return;
        d<Private>()->method = method;
    }

    QMcpPaginatedRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpPaginatedRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QString method;
        QMcpPaginatedRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPPAGINATEDREQUEST_H
