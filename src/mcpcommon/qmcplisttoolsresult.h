// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTTOOLSRESULT_H
#define QMCPLISTTOOLSRESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcptool.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpListToolsResult
    \inmodule QtMcpCommon
    \brief The server's response to a tools/list request from the client.
*/
class Q_MCPCOMMON_EXPORT QMcpListToolsResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpListToolsResult::nextCursor
        \brief An opaque token representing the pagination position after the last returned result.
        If present, there may be more results available.
    */
    Q_PROPERTY(QString nextCursor READ nextCursor WRITE setNextCursor)

    Q_PROPERTY(QList<QMcpTool> tools READ tools WRITE setTools REQUIRED)

public:
    QMcpListToolsResult() : QMcpResult(new Private) {
        qRegisterMetaType<QMcpTool>();
    }

    QString nextCursor() const {
        return d<Private>()->nextCursor;
    }

    void setNextCursor(const QString &cursor) {
        if (nextCursor() == cursor) return;
        d<Private>()->nextCursor = cursor;
    }

    QList<QMcpTool> tools() const {
        return d<Private>()->tools;
    }

    void setTools(const QList<QMcpTool> &tools) {
        if (this->tools() == tools) return;
        d<Private>()->tools = tools;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString nextCursor;
        QList<QMcpTool> tools;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListToolsResult)

QT_END_NAMESPACE

#endif // QMCPLISTTOOLSRESULT_H
