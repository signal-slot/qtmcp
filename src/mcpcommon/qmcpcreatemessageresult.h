// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCREATEMESSAGERESULT_H
#define QMCPCREATEMESSAGERESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpcreatemessageresultcontent.h>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcprole.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCreateMessageResult
    \inmodule QtMcpCommon
    \brief The client's response to a sampling/create_message request from the server. The client should inform the user before returning the sampled message, to allow them to inspect the response (human in the loop) and decide whether to allow the server to see it.
*/
class Q_MCPCOMMON_EXPORT QMcpCreateMessageResult : public QMcpResult
{
    Q_GADGET

    Q_PROPERTY(QMcpCreateMessageResultContent content READ content WRITE setContent REQUIRED)

    /*!
        \property QMcpCreateMessageResult::model
        \brief The name of the model that generated the message.
    */
    Q_PROPERTY(QString model READ model WRITE setModel REQUIRED)

    Q_PROPERTY(QMcpRole::QMcpRole role READ role WRITE setRole REQUIRED)

    /*!
        \property QMcpCreateMessageResult::stopReason
        \brief The reason why sampling stopped, if known.
    */
    Q_PROPERTY(QString stopReason READ stopReason WRITE setStopReason)

public:
    QMcpCreateMessageResult() : QMcpResult(new Private) {}

    QMcpCreateMessageResultContent content() const { return d<Private>()->content; }
    void setContent(const QMcpCreateMessageResultContent &value) {
        if (content() == value) return;
        d<Private>()->content = value;
    }

    QString model() const { return d<Private>()->model; }
    void setModel(const QString &value) {
        if (model() == value) return;
        d<Private>()->model = value;
    }

    QMcpRole::QMcpRole role() const { return d<Private>()->role; }
    void setRole(const QMcpRole::QMcpRole &value) {
        if (role() == value) return;
        d<Private>()->role = value;
    }

    QString stopReason() const { return d<Private>()->stopReason; }
    void setStopReason(const QString &value) {
        if (stopReason() == value) return;
        d<Private>()->stopReason = value;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QMcpCreateMessageResultContent content;
        QString model;
        QMcpRole::QMcpRole role;
        QString stopReason;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCREATEMESSAGERESULT_H
