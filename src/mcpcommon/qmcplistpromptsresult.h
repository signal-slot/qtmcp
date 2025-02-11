// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTPROMPTSRESULT_H
#define QMCPLISTPROMPTSRESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcpprompt.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpListPromptsResult
    \inmodule QtMcpCommon
    \brief The server's response to a prompts/list request from the client.
*/
class Q_MCPCOMMON_EXPORT QMcpListPromptsResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpListPromptsResult::nextCursor
        \brief An opaque token representing the pagination position after the last returned result.
        If present, there may be more results available.
    */
    Q_PROPERTY(QString nextCursor READ nextCursor WRITE setNextCursor)

    Q_PROPERTY(QList<QMcpPrompt> prompts READ prompts WRITE setPrompts REQUIRED)

public:
    QMcpListPromptsResult() : QMcpResult(new Private) {
        qRegisterMetaType<QMcpPrompt>();
    }

    QString nextCursor() const {
        return d<Private>()->nextCursor;
    }

    void setNextCursor(const QString &cursor) {
        if (nextCursor() == cursor) return;
        d<Private>()->nextCursor = cursor;
    }

    QList<QMcpPrompt> prompts() const {
        return d<Private>()->prompts;
    }

    void setPrompts(const QList<QMcpPrompt> &prompts) {
        if (this->prompts() == prompts) return;
        d<Private>()->prompts = prompts;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString nextCursor;
        QList<QMcpPrompt> prompts;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListPromptsResult)

QT_END_NAMESPACE

#endif // QMCPLISTPROMPTSRESULT_H
