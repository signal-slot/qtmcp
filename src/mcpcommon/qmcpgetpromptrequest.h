// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPGETPROMPTREQUEST_H
#define QMCPGETPROMPTREQUEST_H

#include <QtMcpCommon/qmcprequest.h>
#include <QtMcpCommon/qmcpgetpromptrequestparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpGetPromptRequest
    \inmodule QtMcpCommon
    \brief Used by the client to get a prompt provided by the server.
*/
class Q_MCPCOMMON_EXPORT QMcpGetPromptRequest : public QMcpRequest
{
    Q_GADGET

    Q_PROPERTY(QMcpGetPromptRequestParams params READ params WRITE setParams REQUIRED)

public:
    QMcpGetPromptRequest() : QMcpRequest(new Private) {}

    QString method() const final { return "prompts/get"_L1; }

    QMcpGetPromptRequestParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpGetPromptRequestParams &value) {
        if (params() == value) return;
        d<Private>()->params = value;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpRequest::Private {
        QMcpGetPromptRequestParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpGetPromptRequest)

QT_END_NAMESPACE

#endif // QMCPGETPROMPTREQUEST_H
