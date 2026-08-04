// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTCREATETASKRESULT_H
#define QMCPEXTCREATETASKRESULT_H

#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcptaskstatus.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtCreateTaskResult
    \inmodule QtMcpCommon
    \brief The result a server returns in lieu of a standard result to hand out a task handle.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so none of its members are version gated.

    It is deliberately separate from QMcpCreateTaskResult, which implements the
    MCP 2025-11-25 core type of the same schema name: the two are not wire
    compatible. The core version nests the task under a \c task member, whereas
    the extension defines the result as \c {Result & Task} and therefore inlines
    the task members next to the result members. Gadgets support single
    inheritance only, so the QMcpExtTask members are repeated here instead of
    being aggregated; they sit in the same JSON object either way.

    \l resultType is \c "task", which is what lets a client tell this result
    apart from the request's standard result. It is set by the constructor.

    \sa QMcpCreateTaskResult, QMcpExtTask
*/
class Q_MCPCOMMON_EXPORT QMcpExtCreateTaskResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpExtCreateTaskResult::createdAt
        \brief ISO 8601 timestamp when the task was created.
    */
    Q_PROPERTY(QString createdAt READ createdAt WRITE setCreatedAt REQUIRED)

    /*!
        \property QMcpExtCreateTaskResult::lastUpdatedAt
        \brief ISO 8601 timestamp when the task was last updated.
    */
    Q_PROPERTY(QString lastUpdatedAt READ lastUpdatedAt WRITE setLastUpdatedAt REQUIRED)

    /*!
        \property QMcpExtCreateTaskResult::pollIntervalMs
        \brief Suggested polling interval in integer milliseconds.

        The default value is 0, in which case the member is omitted.
    */
    Q_PROPERTY(int pollIntervalMs READ pollIntervalMs WRITE setPollIntervalMs)

    /*!
        \property QMcpExtCreateTaskResult::status
        \brief Current task status.

        This is the seed state of the task, typically though not necessarily
        \c working.
    */
    Q_PROPERTY(QMcpTaskStatus::QMcpTaskStatus status READ status WRITE setStatus REQUIRED)

    /*!
        \property QMcpExtCreateTaskResult::statusMessage
        \brief Optional message describing the current task state.
    */
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage)

    /*!
        \property QMcpExtCreateTaskResult::taskId
        \brief Stable identifier for this task.

        The client uses this value for all subsequent tasks/get, tasks/update
        and tasks/cancel calls.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

    /*!
        \property QMcpExtCreateTaskResult::ttlMs
        \brief Time-to-live duration from creation in integer milliseconds, null for unlimited.

        The schema types this member as "number or null", so it is held as a
        QJsonValue rather than an int. A default constructed QJsonValue is null,
        i.e. unlimited retention.
    */
    Q_PROPERTY(QJsonValue ttlMs READ ttlMs WRITE setTtlMs REQUIRED)

public:
    QMcpExtCreateTaskResult() : QMcpResult(new Private) {
        setResultType("task"_L1);
    }

    QString createdAt() const {
        return d<Private>()->createdAt;
    }

    void setCreatedAt(const QString &createdAt) {
        if (this->createdAt() == createdAt) return;
        d<Private>()->createdAt = createdAt;
    }

    QString lastUpdatedAt() const {
        return d<Private>()->lastUpdatedAt;
    }

    void setLastUpdatedAt(const QString &lastUpdatedAt) {
        if (this->lastUpdatedAt() == lastUpdatedAt) return;
        d<Private>()->lastUpdatedAt = lastUpdatedAt;
    }

    int pollIntervalMs() const {
        return d<Private>()->pollIntervalMs;
    }

    void setPollIntervalMs(int pollIntervalMs) {
        if (this->pollIntervalMs() == pollIntervalMs) return;
        d<Private>()->pollIntervalMs = pollIntervalMs;
    }

    QMcpTaskStatus::QMcpTaskStatus status() const {
        return d<Private>()->status;
    }

    void setStatus(QMcpTaskStatus::QMcpTaskStatus status) {
        if (this->status() == status) return;
        d<Private>()->status = status;
    }

    QString statusMessage() const {
        return d<Private>()->statusMessage;
    }

    void setStatusMessage(const QString &statusMessage) {
        if (this->statusMessage() == statusMessage) return;
        d<Private>()->statusMessage = statusMessage;
    }

    QString taskId() const {
        return d<Private>()->taskId;
    }

    void setTaskId(const QString &taskId) {
        if (this->taskId() == taskId) return;
        d<Private>()->taskId = taskId;
    }

    QJsonValue ttlMs() const {
        return d<Private>()->ttlMs;
    }

    void setTtlMs(const QJsonValue &ttlMs) {
        if (this->ttlMs() == ttlMs) return;
        d<Private>()->ttlMs = ttlMs;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString createdAt;
        QString lastUpdatedAt;
        int pollIntervalMs = 0;
        // A task that is reported at all has been started, so "working" is the
        // only sensible default for this REQUIRED member.
        QMcpTaskStatus::QMcpTaskStatus status = QMcpTaskStatus::working;
        QString statusMessage;
        QString taskId;
        QJsonValue ttlMs;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpExtCreateTaskResult)

QT_END_NAMESPACE

#endif // QMCPEXTCREATETASKRESULT_H
