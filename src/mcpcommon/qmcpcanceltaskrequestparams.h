// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCANCELTASKREQUESTPARAMS_H
#define QMCPCANCELTASKREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCancelTaskRequestParams
    \inmodule QtMcpCommon

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpCancelTaskRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpCancelTaskRequestParams::taskId
        \brief The task identifier to cancel.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

public:
    QMcpCancelTaskRequestParams() : QMcpGadget(new Private) {}

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

Q_DECLARE_SHARED(QMcpCancelTaskRequestParams)

QT_END_NAMESPACE

#endif // QMCPCANCELTASKREQUESTPARAMS_H
