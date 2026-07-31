// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTGETTASKRESULT_H
#define QMCPEXTGETTASKRESULT_H

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcptaskstatus.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtGetTaskResult
    \inmodule QtMcpCommon
    \brief The response to a tasks/get request of the tasks extension.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so none of its members are version gated.

    It is deliberately separate from QMcpGetTaskResult, which implements the MCP
    2025-11-25 core type of the same schema name: the two are not wire
    compatible. The extension renamed \c ttl to \c ttlMs and \c pollInterval to
    \c pollIntervalMs, and it inlines a status specific payload that the core
    type does not have.

    The schema defines the result as \c {Result & DetailedTask}, where
    \c DetailedTask is an anyOf over five per-status shapes. Gadgets model
    neither an allOf nor an anyOf, so the task members are repeated here next to
    the result members and the payload of every variant is exposed as an
    optional member. Which one carries a value follows from \l status:

    \list
    \li \c working, \c cancelled: no payload
    \li \c input_required: \l inputRequests
    \li \c completed: \l result
    \li \c failed: \l error
    \endlist

    The schema marks each payload required for its own variant, but a single
    gadget cannot make a member conditionally required, so all three are
    optional here and the caller is responsible for setting the one that matches
    \l status.

    \l resultType is \c "complete": this is the standard result shape of
    tasks/get, not a task handle.

    \sa QMcpGetTaskResult, QMcpExtTask, QMcpGetTaskRequest
*/
class Q_MCPCOMMON_EXPORT QMcpExtGetTaskResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpExtGetTaskResult::createdAt
        \brief ISO 8601 timestamp when the task was created.
    */
    Q_PROPERTY(QString createdAt READ createdAt WRITE setCreatedAt REQUIRED)

    /*!
        \property QMcpExtGetTaskResult::error
        \brief The JSON-RPC error that caused the task to fail.

        Set when \l status is \c failed, which the extension reserves for
        JSON-RPC errors during execution. A tool result with \c {isError: true}
        is not a failure in that sense; it arrives as \l result with \l status
        \c completed instead.

        The schema types this member as a free-form JSON object, so it is kept
        as a raw QJsonObject.
    */
    Q_PROPERTY(QJsonObject error READ error WRITE setError)

    /*!
        \property QMcpExtGetTaskResult::inputRequests
        \brief Server-to-client requests that need to be fulfilled during task execution.

        Set when \l status is \c input_required. Keys are arbitrary identifiers
        for matching requests to responses, and each must be unique over the
        lifetime of a task. The client answers them through
        QMcpExtUpdateTaskRequestParams::inputResponses.

        As with QMcpInputRequiredResult::inputRequests, the key set is server
        defined and every value is an anyOf over CreateMessageRequest,
        ListRootsRequest and ElicitRequest, which has no faithful gadget
        representation; the member is therefore kept as a raw QJsonObject and
        callers pick the variant apart themselves.
    */
    Q_PROPERTY(QJsonObject inputRequests READ inputRequests WRITE setInputRequests)

    /*!
        \property QMcpExtGetTaskResult::lastUpdatedAt
        \brief ISO 8601 timestamp when the task was last updated.
    */
    Q_PROPERTY(QString lastUpdatedAt READ lastUpdatedAt WRITE setLastUpdatedAt REQUIRED)

    /*!
        \property QMcpExtGetTaskResult::pollIntervalMs
        \brief Suggested polling interval in integer milliseconds.

        The default value is 0, in which case the member is omitted.
    */
    Q_PROPERTY(int pollIntervalMs READ pollIntervalMs WRITE setPollIntervalMs)

    /*!
        \property QMcpExtGetTaskResult::result
        \brief The final result of the task.

        Set when \l status is \c completed. The structure matches the result type
        of the original request, for example QMcpCallToolResult for a task that
        augmented a tools/call request. Since the shape depends on the request
        the task was created for, it is kept as a raw QJsonObject.
    */
    Q_PROPERTY(QJsonObject result READ result WRITE setResult)

    /*!
        \property QMcpExtGetTaskResult::status
        \brief Current task status.
    */
    Q_PROPERTY(QMcpTaskStatus::QMcpTaskStatus status READ status WRITE setStatus REQUIRED)

    /*!
        \property QMcpExtGetTaskResult::statusMessage
        \brief Optional message describing the current task state.
    */
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage)

    /*!
        \property QMcpExtGetTaskResult::taskId
        \brief Stable identifier for this task.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

    /*!
        \property QMcpExtGetTaskResult::ttlMs
        \brief Time-to-live duration from creation in integer milliseconds, null for unlimited.

        The schema types this member as "number or null", so it is held as a
        QJsonValue rather than an int. A default constructed QJsonValue is null,
        i.e. unlimited retention.
    */
    Q_PROPERTY(QJsonValue ttlMs READ ttlMs WRITE setTtlMs REQUIRED)

public:
    QMcpExtGetTaskResult() : QMcpResult(new Private) {}

    QString createdAt() const {
        return d<Private>()->createdAt;
    }

    void setCreatedAt(const QString &createdAt) {
        if (this->createdAt() == createdAt) return;
        d<Private>()->createdAt = createdAt;
    }

    QJsonObject error() const {
        return d<Private>()->error;
    }

    void setError(const QJsonObject &error) {
        if (this->error() == error) return;
        d<Private>()->error = error;
    }

    QJsonObject inputRequests() const {
        return d<Private>()->inputRequests;
    }

    void setInputRequests(const QJsonObject &inputRequests) {
        if (this->inputRequests() == inputRequests) return;
        d<Private>()->inputRequests = inputRequests;
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

    QJsonObject result() const {
        return d<Private>()->result;
    }

    void setResult(const QJsonObject &result) {
        if (this->result() == result) return;
        d<Private>()->result = result;
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
        QJsonObject error;
        QJsonObject inputRequests;
        QString lastUpdatedAt;
        int pollIntervalMs = 0;
        QJsonObject result;
        // A task that is reported at all has been started, so "working" is the
        // only sensible default for this REQUIRED member.
        QMcpTaskStatus::QMcpTaskStatus status = QMcpTaskStatus::working;
        QString statusMessage;
        QString taskId;
        QJsonValue ttlMs;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpExtGetTaskResult)

QT_END_NAMESPACE

#endif // QMCPEXTGETTASKRESULT_H
