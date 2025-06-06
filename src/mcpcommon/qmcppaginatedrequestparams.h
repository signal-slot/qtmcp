// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPAGINATEDREQUESTPARAMS_H
#define QMCPPAGINATEDREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPaginatedRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpPaginatedRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpPaginatedRequestParams::cursor
        \brief An opaque token representing the current pagination position.
        If provided, the server should return results starting after this cursor.
    */
    Q_PROPERTY(QString cursor READ cursor WRITE setCursor)

public:
    QMcpPaginatedRequestParams() : QMcpGadget(new Private) {}

    QString cursor() const {
        return d<Private>()->cursor;
    }

    void setCursor(const QString &cursor) {
        if (this->cursor() == cursor) return;
        d<Private>()->cursor = cursor;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString cursor;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPaginatedRequestParams)

QT_END_NAMESPACE

#endif // QMCPPAGINATEDREQUESTPARAMS_H
