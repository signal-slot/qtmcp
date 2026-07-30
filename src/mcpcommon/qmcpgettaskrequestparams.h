// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPGETTASKREQUESTPARAMS_H
#define QMCPGETTASKREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpGetTaskRequestParams
    \inmodule QtMcpCommon

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpGetTaskRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpGetTaskRequestParams::taskId
        \brief The task identifier to query.
    */
    Q_PROPERTY(QString taskId READ taskId WRITE setTaskId REQUIRED)

public:
    QMcpGetTaskRequestParams() : QMcpGadget(new Private) {}

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

Q_DECLARE_SHARED(QMcpGetTaskRequestParams)

QT_END_NAMESPACE

#endif // QMCPGETTASKREQUESTPARAMS_H
