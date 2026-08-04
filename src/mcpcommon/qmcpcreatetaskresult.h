// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCREATETASKRESULT_H
#define QMCPCREATETASKRESULT_H

#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcptask.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCreateTaskResult
    \inmodule QtMcpCommon
    \brief A response to a task-augmented request.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpCreateTaskResult : public QMcpResult
{
    Q_GADGET

    Q_PROPERTY(QMcpTask task READ task WRITE setTask REQUIRED)

public:
    QMcpCreateTaskResult() : QMcpResult(new Private) {}

    QMcpTask task() const {
        return d<Private>()->task;
    }

    void setTask(const QMcpTask &task) {
        if (this->task() == task) return;
        d<Private>()->task = task;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QMcpTask task;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCreateTaskResult)

QT_END_NAMESPACE

#endif // QMCPCREATETASKRESULT_H
