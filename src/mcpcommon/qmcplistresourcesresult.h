// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTRESOURCESRESULT_H
#define QMCPLISTRESOURCESRESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcpresource.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpListResourcesResult
    \inmodule QtMcpCommon
    \brief The server's response to a resources/list request from the client.
*/
class Q_MCPCOMMON_EXPORT QMcpListResourcesResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpListResourcesResult::nextCursor
        \brief An opaque token representing the pagination position after the last returned result.
        If present, there may be more results available.
    */
    Q_PROPERTY(QString nextCursor READ nextCursor WRITE setNextCursor)

    Q_PROPERTY(QList<QMcpResource> resources READ resources WRITE setResources REQUIRED)

public:
    QMcpListResourcesResult() : QMcpResult(new Private) {
        qRegisterMetaType<QMcpResource>();
    }

    QString nextCursor() const {
        return d<Private>()->nextCursor;
    }

    void setNextCursor(const QString &cursor) {
        if (nextCursor() == cursor) return;
        d<Private>()->nextCursor = cursor;
    }

    QList<QMcpResource> resources() const {
        return d<Private>()->resources;
    }

    void setResources(const QList<QMcpResource> &resources) {
        if (this->resources() == resources) return;
        d<Private>()->resources = resources;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString nextCursor;
        QList<QMcpResource> resources;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListResourcesResult)

QT_END_NAMESPACE

#endif // QMCPLISTRESOURCESRESULT_H
