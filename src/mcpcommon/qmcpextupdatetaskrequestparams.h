// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEXTUPDATETASKREQUESTPARAMS_H
#define QMCPEXTUPDATETASKREQUESTPARAMS_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpExtUpdateTaskRequestParams
    \inmodule QtMcpCommon
    \brief The parameters of a tasks/update request.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so none of its members are version gated. MCP
    2025-11-25 has no tasks/update method, so there is no core counterpart this
    could have extended.

    \sa QMcpExtUpdateTaskRequest, QMcpExtGetTaskResult
*/
class Q_MCPCOMMON_EXPORT QMcpExtUpdateTaskRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpExtUpdateTaskRequestParams::inputResponses
        \brief Responses to the outstanding input requests of the task.

        Each key must correspond to a key that is currently outstanding in
        QMcpExtGetTaskResult::inputRequests. A server ignores responses for
        unknown or already answered keys, and it may accept a strict subset, in
        which case the task stays in the \c input_required status until the
        remaining responses arrive.

        The schema types every value as an anyOf over the three server-to-client
        response shapes and leaves the key set to the server, so this is kept as
        a raw QJsonObject rather than a typed map, mirroring
        QMcpExtGetTaskResult::inputRequests.
    */
    Q_PROPERTY(QJsonObject inputResponses READ inputResponses WRITE setInputResponses REQUIRED)

    /*!
        \property QMcpExtUpdateTaskRequestParams::taskId
        \brief Identifier of the task to update.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

public:
    QMcpExtUpdateTaskRequestParams() : QMcpGadget(new Private) {}

    QJsonObject inputResponses() const {
        return d<Private>()->inputResponses;
    }

    void setInputResponses(const QJsonObject &inputResponses) {
        if (this->inputResponses() == inputResponses) return;
        d<Private>()->inputResponses = inputResponses;
    }

    QString taskId() const {
        return d<Private>()->taskId;
    }

    void setTaskId(const QString &taskId) {
        if (this->taskId() == taskId) return;
        d<Private>()->taskId = taskId;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject inputResponses;
        QString taskId;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpExtUpdateTaskRequestParams)

QT_END_NAMESPACE

#endif // QMCPEXTUPDATETASKREQUESTPARAMS_H
