// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTPROMPTSREQUESTPARAMS_H
#define QMCPLISTPROMPTSREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpListPromptsRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpListPromptsRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpListPromptsRequestParams::cursor
        \brief An opaque token representing the current pagination position.
        If provided, the server should return results starting after this cursor.
    */
    Q_PROPERTY(QString cursor READ cursor WRITE setCursor)

public:
    QMcpListPromptsRequestParams() : QMcpGadget(new Private) {}

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

QT_END_NAMESPACE

#endif // QMCPLISTPROMPTSREQUESTPARAMS_H
