// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPELICITATIONCOMPLETENOTIFICATIONPARAMS_H
#define QMCPELICITATIONCOMPLETENOTIFICATIONPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpnotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpElicitationCompleteNotificationParams
    \inmodule QtMcpCommon
    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpElicitationCompleteNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpElicitationCompleteNotificationParams::elicitationId
        \brief The ID of the elicitation that completed.

        This is the elicitationId the server sent with the QMcpElicitRequest.
    */
    Q_PROPERTY(QString elicitationId READ elicitationId WRITE setElicitationId REQUIRED)

public:
    QMcpElicitationCompleteNotificationParams() : QMcpNotificationParams(new Private) {}

    QString elicitationId() const {
        return d<Private>()->elicitationId;
    }

    void setElicitationId(const QString &elicitationId) {
        if (this->elicitationId() == elicitationId) return;
        d<Private>()->elicitationId = elicitationId;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QString elicitationId;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpElicitationCompleteNotificationParams)

QT_END_NAMESPACE

#endif // QMCPELICITATIONCOMPLETENOTIFICATIONPARAMS_H
