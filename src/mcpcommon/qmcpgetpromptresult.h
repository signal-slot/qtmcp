// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPGETPROMPTRESULT_H
#define QMCPGETPROMPTRESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcppromptmessage.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpGetPromptResult
    \inmodule QtMcpCommon
    \brief The server's response to a prompts/get request from the client.
*/
class Q_MCPCOMMON_EXPORT QMcpGetPromptResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpGetPromptResult::description
        \brief An optional description for the prompt.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    Q_PROPERTY(QList<QMcpPromptMessage> messages READ messages WRITE setMessages REQUIRED)

public:
    QMcpGetPromptResult() : QMcpResult(new Private) {
        qRegisterMetaType<QMcpPromptMessage>();
    }

    QString description() const {
        return d<Private>()->description;
    }

    void setDescription(const QString &value) {
        if (description() == value) return;
        d<Private>()->description = value;
    }

    QList<QMcpPromptMessage> messages() const {
        return d<Private>()->messages;
    }

    void setMessages(const QList<QMcpPromptMessage> &value) {
        if (messages() == value) return;
        d<Private>()->messages = value;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString description;
        QList<QMcpPromptMessage> messages;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpGetPromptResult)

QT_END_NAMESPACE

#endif // QMCPGETPROMPTRESULT_H
