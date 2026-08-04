// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTTASKSTATUSNOTIFICATIONPARAMS_H
#define QMCPEXTTASKSTATUSNOTIFICATIONPARAMS_H

#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcptaskstatus.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtTaskStatusNotificationParams
    \inmodule QtMcpCommon
    \brief The parameters of a notifications/tasks notification.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so none of its members are version gated.

    It is deliberately separate from QMcpTaskStatusNotificationParams, which
    implements the MCP 2025-11-25 core type of the same schema name: the two are
    not wire compatible. The extension renamed \c ttl to \c ttlMs and
    \c pollInterval to \c pollIntervalMs, and it inlines the status specific
    payload of \c DetailedTask that the core type does not have.

    The schema defines the parameters as an allOf of NotificationParams and
    \c DetailedTask, an anyOf over five per-status shapes. Gadgets model neither,
    so the task members are repeated here next to the notification parameters
    and every variant payload is exposed as an optional member, selected by
    \l status exactly as in QMcpExtGetTaskResult. Each notification carries the
    complete task, identical to what tasks/get would have returned at that
    moment.

    \sa QMcpTaskStatusNotificationParams, QMcpExtTaskStatusNotification,
        QMcpExtGetTaskResult
*/
class Q_MCPCOMMON_EXPORT QMcpExtTaskStatusNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpExtTaskStatusNotificationParams::createdAt
        \brief ISO 8601 timestamp when the task was created.
    */
    Q_PROPERTY(QString createdAt READ createdAt WRITE setCreatedAt REQUIRED)

    /*!
        \property QMcpExtTaskStatusNotificationParams::error
        \brief The JSON-RPC error that caused the task to fail.

        Set when \l status is \c failed. Kept as a raw QJsonObject, as the schema
        types it as a free-form JSON object.
    */
    Q_PROPERTY(QJsonObject error READ error WRITE setError)

    /*!
        \property QMcpExtTaskStatusNotificationParams::inputRequests
        \brief Server-to-client requests that need to be fulfilled during task execution.

        Set when \l status is \c input_required. Kept as a raw QJsonObject for
        the same reason as QMcpExtGetTaskResult::inputRequests.
    */
    Q_PROPERTY(QJsonObject inputRequests READ inputRequests WRITE setInputRequests)

    /*!
        \property QMcpExtTaskStatusNotificationParams::lastUpdatedAt
        \brief ISO 8601 timestamp when the task was last updated.
    */
    Q_PROPERTY(QString lastUpdatedAt READ lastUpdatedAt WRITE setLastUpdatedAt REQUIRED)

    /*!
        \property QMcpExtTaskStatusNotificationParams::pollIntervalMs
        \brief Suggested polling interval in integer milliseconds.

        The default value is 0, in which case the member is omitted.
    */
    Q_PROPERTY(int pollIntervalMs READ pollIntervalMs WRITE setPollIntervalMs)

    /*!
        \property QMcpExtTaskStatusNotificationParams::result
        \brief The final result of the task.

        Set when \l status is \c completed. Its shape follows the result type of
        the request the task augmented, so it is kept as a raw QJsonObject.
    */
    Q_PROPERTY(QJsonObject result READ result WRITE setResult)

    /*!
        \property QMcpExtTaskStatusNotificationParams::status
        \brief Current task status.
    */
    Q_PROPERTY(QMcpTaskStatus::QMcpTaskStatus status READ status WRITE setStatus REQUIRED)

    /*!
        \property QMcpExtTaskStatusNotificationParams::statusMessage
        \brief Optional message describing the current task state.
    */
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage)

    /*!
        \property QMcpExtTaskStatusNotificationParams::taskId
        \brief Stable identifier for this task.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

    /*!
        \property QMcpExtTaskStatusNotificationParams::ttlMs
        \brief Time-to-live duration from creation in integer milliseconds, null for unlimited.

        The schema types this member as "number or null", so it is held as a
        QJsonValue rather than an int. A default constructed QJsonValue is null,
        i.e. unlimited retention.
    */
    Q_PROPERTY(QJsonValue ttlMs READ ttlMs WRITE setTtlMs REQUIRED)

public:
    QMcpExtTaskStatusNotificationParams() : QMcpNotificationParams(new Private) {}

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
    struct Private : public QMcpNotificationParams::Private {
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

Q_DECLARE_SHARED(QMcpExtTaskStatusNotificationParams)

QT_END_NAMESPACE

#endif // QMCPEXTTASKSTATUSNOTIFICATIONPARAMS_H
