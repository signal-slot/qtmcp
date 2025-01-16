// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCOMPLETERESULTCOMPLETION_H
#define QMCPCOMPLETERESULTCOMPLETION_H

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCompleteResultCompletion
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpCompleteResultCompletion : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpCompleteResultCompletion::hasMore
        \brief Indicates whether there are additional completion options beyond those provided in the current response, even if the exact total is unknown.

        The default value is false.
    */
    Q_PROPERTY(bool hasMore READ hasMore WRITE setHasMore)

    /*!
        \property QMcpCompleteResultCompletion::total
        \brief The total number of completion options available. This can exceed the number of values actually sent in the response.

        The default value is 0.
    */
    Q_PROPERTY(int total READ total WRITE setTotal)

    /*!
        \property QMcpCompleteResultCompletion::values
        \brief An array of completion values. Must not exceed 100 items.
    */
    Q_PROPERTY(QList<QString> values READ values WRITE setValues REQUIRED)

public:
    QMcpCompleteResultCompletion() : QMcpGadget(new Private) {}

    bool hasMore() const {
        return d<Private>()->hasMore;
    }

    void setHasMore(bool hasMore) {
        if (this->hasMore() == hasMore) return;
        d<Private>()->hasMore = hasMore;
    }

    int total() const {
        return d<Private>()->total;
    }

    void setTotal(int total) {
        if (this->total() == total) return;
        d<Private>()->total = total;
    }

    QList<QString> values() const {
        return d<Private>()->values;
    }

    void setValues(const QList<QString> &values) {
        if (this->values() == values) return;
        d<Private>()->values = values;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        bool hasMore = false;
        int total = 0;
        QList<QString> values;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCOMPLETERESULTCOMPLETION_H
