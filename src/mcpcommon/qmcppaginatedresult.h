// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPAGINATEDRESULT_H
#define QMCPPAGINATEDRESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPaginatedResult
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpPaginatedResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpPaginatedResult::nextCursor
        \brief An opaque token representing the pagination position after the last returned result.
        If present, there may be more results available.
    */
    Q_PROPERTY(QString nextCursor READ nextCursor WRITE setNextCursor)

public:
    QMcpPaginatedResult() : QMcpResult(new Private) {}

    QString nextCursor() const {
        return d<Private>()->nextCursor;
    }

    void setNextCursor(const QString &cursor) {
        if (nextCursor() == cursor) return;
        d<Private>()->nextCursor = cursor;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString nextCursor;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPaginatedResult)

QT_END_NAMESPACE

#endif // QMCPPAGINATEDRESULT_H
