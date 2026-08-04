// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTTASK_H
#define QMCPEXTTASK_H

#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcptaskstatus.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtTask
    \inmodule QtMcpCommon
    \brief Operational metadata about ongoing work, as defined by the tasks extension.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision. The extension is enabled through capability
    negotiation and not by the negotiated protocol version, so none of its
    members are version gated.

    It is deliberately separate from QMcpTask, which implements the MCP
    2025-11-25 core \c Task: the two are not wire compatible. The extension
    renamed \c ttl to \c ttlMs and \c pollInterval to \c pollIntervalMs, and
    since QMcpGadget derives the JSON key from the Q_PROPERTY name, a single
    gadget cannot serve both spellings.

    \sa QMcpTask
*/
class Q_MCPCOMMON_EXPORT QMcpExtTask : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpExtTask::createdAt
        \brief ISO 8601 timestamp when the task was created.
    */
    Q_PROPERTY(QString createdAt READ createdAt WRITE setCreatedAt REQUIRED)

    /*!
        \property QMcpExtTask::lastUpdatedAt
        \brief ISO 8601 timestamp when the task was last updated.
    */
    Q_PROPERTY(QString lastUpdatedAt READ lastUpdatedAt WRITE setLastUpdatedAt REQUIRED)

    /*!
        \property QMcpExtTask::pollIntervalMs
        \brief Suggested polling interval in integer milliseconds.

        Clients should honor this value to avoid overwhelming the server. It may
        change over the lifetime of a task. The default value is 0, in which
        case the member is omitted.
    */
    Q_PROPERTY(int pollIntervalMs READ pollIntervalMs WRITE setPollIntervalMs)

    /*!
        \property QMcpExtTask::status
        \brief Current task status.

        The extension defines the same five states as MCP 2025-11-25, so
        QMcpTaskStatus is reused verbatim.
    */
    Q_PROPERTY(QMcpTaskStatus::QMcpTaskStatus status READ status WRITE setStatus REQUIRED)

    /*!
        \property QMcpExtTask::statusMessage
        \brief Optional message describing the current task state.

        This can provide context for any status, for example:
        \list
        \li Progress descriptions for "working"
        \li Work blocked on "input_required"
        \li Reasons for "cancelled" status
        \li Summaries for "completed" status
        \li Additional information for "failed" status
        \endlist
    */
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage)

    /*!
        \property QMcpExtTask::taskId
        \brief Stable identifier for this task.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

    /*!
        \property QMcpExtTask::ttlMs
        \brief Time-to-live duration from creation in integer milliseconds, null for unlimited.

        The server may discard the task after the TTL elapses, and the value may
        change over the lifetime of a task. The schema types this member as
        "number or null", so it is held as a QJsonValue rather than an int. A
        default constructed QJsonValue is null, i.e. unlimited retention.
    */
    Q_PROPERTY(QJsonValue ttlMs READ ttlMs WRITE setTtlMs REQUIRED)

public:
    QMcpExtTask() : QMcpGadget(new Private) {}

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
    struct Private : public QMcpGadget::Private {
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

Q_DECLARE_SHARED(QMcpExtTask)

QT_END_NAMESPACE

#endif // QMCPEXTTASK_H
