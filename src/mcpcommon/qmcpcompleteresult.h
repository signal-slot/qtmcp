// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCOMPLETERESULT_H
#define QMCPCOMPLETERESULT_H

#include <QtMcpCommon/qmcpcompleteresultcompletion.h>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCompleteResult
    \inmodule QtMcpCommon
    \brief The server's response to a completion/complete request
*/
class Q_MCPCOMMON_EXPORT QMcpCompleteResult : public QMcpResult
{
    Q_GADGET

    Q_PROPERTY(QMcpCompleteResultCompletion completion READ completion WRITE setCompletion REQUIRED)

public:
    QMcpCompleteResult() : QMcpResult(new Private) {}

    QMcpCompleteResultCompletion completion() const {
        return d<Private>()->completion;
    }

    void setCompletion(const QMcpCompleteResultCompletion &completion) {
        if (this->completion() == completion) return;
        d<Private>()->completion = completion;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QMcpCompleteResultCompletion completion;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCOMPLETERESULT_H
