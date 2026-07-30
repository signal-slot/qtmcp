// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRELATEDTASKMETADATA_H
#define QMCPRELATEDTASKMETADATA_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpRelatedTaskMetadata
    \inmodule QtMcpCommon
    \brief Metadata for associating messages with a task.

    Include this in the \c _meta field under the key
    \c io.modelcontextprotocol/related-task.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpRelatedTaskMetadata : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpRelatedTaskMetadata::taskId
        \brief The task identifier this message is associated with.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

public:
    QMcpRelatedTaskMetadata() : QMcpGadget(new Private) {}

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
        QString taskId;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpRelatedTaskMetadata)

QT_END_NAMESPACE

#endif // QMCPRELATEDTASKMETADATA_H
