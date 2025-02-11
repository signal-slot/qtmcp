// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPREADRESOURCERESULT_H
#define QMCPREADRESOURCERESULT_H

#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcpreadresourceresultcontents.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpReadResourceResult
    \inmodule QtMcpCommon
    \brief The server's response to a resources/read request from the client.
*/
class Q_MCPCOMMON_EXPORT QMcpReadResourceResult : public QMcpResult
{
    Q_GADGET

    Q_PROPERTY(QList<QMcpReadResourceResultContents> contents READ contents WRITE setContents REQUIRED)

public:
    QMcpReadResourceResult() : QMcpResult(new Private) {
        qRegisterMetaType<QMcpReadResourceResultContents>();
    }

    QList<QMcpReadResourceResultContents> contents() const {
        return d<Private>()->contents;
    }

    void setContents(const QList<QMcpReadResourceResultContents> &contents) {
        if (this->contents() == contents) return;
        d<Private>()->contents = contents;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }
public:
    struct Private : public QMcpResult::Private {
        QList<QMcpReadResourceResultContents> contents;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpReadResourceResult)

QT_END_NAMESPACE

#endif // QMCPREADRESOURCERESULT_H
