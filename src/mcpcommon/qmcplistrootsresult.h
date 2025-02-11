// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLISTROOTSRESULT_H
#define QMCPLISTROOTSRESULT_H

#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcproot.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpListRootsResult
    \inmodule QtMcpCommon
    \brief The client's response to a roots/list request from the server.
    This result contains an array of Root objects, each representing a root directory
    or file that the server can operate on.
*/
class Q_MCPCOMMON_EXPORT QMcpListRootsResult : public QMcpResult
{
    Q_GADGET

    Q_PROPERTY(QList<QMcpRoot> roots READ roots WRITE setRoots REQUIRED)

public:
    QMcpListRootsResult() : QMcpResult(new Private) {}

    QList<QMcpRoot> roots() const {
        return d<Private>()->roots;
    }

    void setRoots(const QList<QMcpRoot> &roots) {
        if (this->roots() == roots) return;
        d<Private>()->roots = roots;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QList<QMcpRoot> roots;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpListRootsResult)

QT_END_NAMESPACE

#endif // QMCPLISTROOTSRESULT_H
