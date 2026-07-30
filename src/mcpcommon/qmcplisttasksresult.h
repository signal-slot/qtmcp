// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTTASKSRESULT_H
#define QMCPLISTTASKSRESULT_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcptask.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListTasksResult
    \inmodule QtMcpCommon
    \brief The response to a tasks/list request.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpListTasksResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpListTasksResult::nextCursor
        \brief An opaque token representing the pagination position after the last returned result.
        If present, there may be more results available.
    */
    Q_PROPERTY(QString nextCursor READ nextCursor WRITE setNextCursor)

    Q_PROPERTY(QList<QMcpTask> tasks READ tasks WRITE setTasks REQUIRED)

public:
    QMcpListTasksResult() : QMcpResult(new Private) {
        qRegisterMetaType<QMcpTask>();
    }

    QString nextCursor() const {
        return d<Private>()->nextCursor;
    }

    void setNextCursor(const QString &cursor) {
        if (nextCursor() == cursor) return;
        d<Private>()->nextCursor = cursor;
    }

    QList<QMcpTask> tasks() const {
        return d<Private>()->tasks;
    }

    void setTasks(const QList<QMcpTask> &tasks) {
        if (this->tasks() == tasks) return;
        d<Private>()->tasks = tasks;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString nextCursor;
        QList<QMcpTask> tasks;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListTasksResult)

QT_END_NAMESPACE

#endif // QMCPLISTTASKSRESULT_H
