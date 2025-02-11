// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTPROMPTSREQUEST_H
#define QMCPLISTPROMPTSREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcplistpromptsrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListPromptsRequest
    \inmodule QtMcpCommon
    \brief Sent from the client to request a list of prompts and prompt templates the server has.
*/
class Q_MCPCOMMON_EXPORT QMcpListPromptsRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpListPromptsRequestParams params READ params WRITE setParams)

public:
    QMcpListPromptsRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "prompts/list"_L1; }

    QMcpListPromptsRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpListPromptsRequestParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpListPromptsRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListPromptsRequest)

QT_END_NAMESPACE

#endif // QMCPLISTPROMPTSREQUEST_H
