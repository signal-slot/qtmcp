// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTRESOURCETEMPLATESRESULT_H
#define QMCPLISTRESOURCETEMPLATESRESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcpresourcetemplate.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpListResourceTemplatesResult
    \inmodule QtMcpCommon
    \brief The server's response to a resources/templates/list request from the client.
*/
class Q_MCPCOMMON_EXPORT QMcpListResourceTemplatesResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpListResourceTemplatesResult::nextCursor
        \brief An opaque token representing the pagination position after the last returned result.
        If present, there may be more results available.
    */
    Q_PROPERTY(QString nextCursor READ nextCursor WRITE setNextCursor)

    Q_PROPERTY(QList<QMcpResourceTemplate> resourceTemplates READ resourceTemplates WRITE setResourceTemplates REQUIRED)

public:
    QMcpListResourceTemplatesResult() : QMcpResult(new Private) {
        qRegisterMetaType<QMcpResourceTemplate>();
    }

    QString nextCursor() const {
        return d<Private>()->nextCursor;
    }

    void setNextCursor(const QString &cursor) {
        if (nextCursor() == cursor) return;
        d<Private>()->nextCursor = cursor;
    }

    QList<QMcpResourceTemplate> resourceTemplates() const {
        return d<Private>()->resourceTemplates;
    }

    void setResourceTemplates(const QList<QMcpResourceTemplate> &templates) {
        if (resourceTemplates() == templates) return;
        d<Private>()->resourceTemplates = templates;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString nextCursor;
        QList<QMcpResourceTemplate> resourceTemplates;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPLISTRESOURCETEMPLATESRESULT_H
