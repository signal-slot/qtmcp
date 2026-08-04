// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCANCELTASKRESULT_H
#define QMCPCANCELTASKRESULT_H

#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcptaskstatus.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCancelTaskResult
    \inmodule QtMcpCommon
    \brief The response to a tasks/cancel request.

    The schema defines this type as an allOf of Result and Task. Gadgets support
    single inheritance only, so the Task members are repeated here instead of
    being aggregated; they sit next to the Result members in the JSON object
    either way.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpCancelTaskResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpCancelTaskResult::createdAt
        \brief ISO 8601 timestamp when the task was created.
    */
    Q_PROPERTY(QString createdAt READ createdAt WRITE setCreatedAt REQUIRED)

    /*!
        \property QMcpCancelTaskResult::lastUpdatedAt
        \brief ISO 8601 timestamp when the task was last updated.
    */
    Q_PROPERTY(QString lastUpdatedAt READ lastUpdatedAt WRITE setLastUpdatedAt REQUIRED)

    /*!
        \property QMcpCancelTaskResult::pollInterval
        \brief Suggested polling interval in milliseconds.

        The default value is 0, in which case the member is omitted.
    */
    Q_PROPERTY(int pollInterval READ pollInterval WRITE setPollInterval)

    /*!
        \property QMcpCancelTaskResult::status
        \brief Current task state.
    */
    Q_PROPERTY(QMcpTaskStatus::QMcpTaskStatus status READ status WRITE setStatus REQUIRED)

    /*!
        \property QMcpCancelTaskResult::statusMessage
        \brief Optional human-readable message describing the current task state.
    */
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage)

    /*!
        \property QMcpCancelTaskResult::taskId
        \brief The task identifier.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

    /*!
        \property QMcpCancelTaskResult::ttl
        \brief Actual retention duration from creation in milliseconds, null for unlimited.

        The schema types this member as "integer or null", so it is held as a
        QJsonValue rather than an int. A default constructed QJsonValue is null,
        i.e. unlimited retention.
    */
    Q_PROPERTY(QJsonValue ttl READ ttl WRITE setTtl REQUIRED)

public:
    QMcpCancelTaskResult() : QMcpResult(new Private) {}

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

    int pollInterval() const {
        return d<Private>()->pollInterval;
    }

    void setPollInterval(int pollInterval) {
        if (this->pollInterval() == pollInterval) return;
        d<Private>()->pollInterval = pollInterval;
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

    QJsonValue ttl() const {
        return d<Private>()->ttl;
    }

    void setTtl(const QJsonValue &ttl) {
        if (this->ttl() == ttl) return;
        d<Private>()->ttl = ttl;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString createdAt;
        QString lastUpdatedAt;
        int pollInterval = 0;
        // A task that is reported at all has been started, so "working" is the
        // only sensible default for this REQUIRED member.
        QMcpTaskStatus::QMcpTaskStatus status = QMcpTaskStatus::working;
        QString statusMessage;
        QString taskId;
        QJsonValue ttl;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCancelTaskResult)

QT_END_NAMESPACE

#endif // QMCPCANCELTASKRESULT_H
