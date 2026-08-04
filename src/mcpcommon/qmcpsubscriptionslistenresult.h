// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSUBSCRIPTIONSLISTENRESULT_H
#define QMCPSUBSCRIPTIONSLISTENRESULT_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSubscriptionsListenResult
    \inmodule QtMcpCommon
    \brief The response to a subscriptions/listen request, signalling that the subscription has ended gracefully.

    Because the listen stream is long-lived, this result is sent only when the
    server tears the subscription down (for example, during server shutdown);
    an abrupt transport close carries no response. The result body is otherwise
    empty: the subscription stream identifier travels in
    \c io.modelcontextprotocol/subscriptionId inside the inherited
    QMcpResult::_meta.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpSubscriptionsListenResult : public QMcpResult
{
    Q_GADGET

public:
    QMcpSubscriptionsListenResult() : QMcpResult(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSubscriptionsListenResult)

QT_END_NAMESPACE

#endif // QMCPSUBSCRIPTIONSLISTENRESULT_H
