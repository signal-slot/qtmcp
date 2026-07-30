// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSUBSCRIPTIONFILTER_H
#define QMCPSUBSCRIPTIONFILTER_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSubscriptionFilter
    \inmodule QtMcpCommon
    \brief The set of notification types a client may opt in to on a subscriptions/listen request.

    Each notification type is opt-in; the server must not send notification
    types the client has not explicitly requested here.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpSubscriptionFilter : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpSubscriptionFilter::toolsListChanged
        \brief If true, receive notifications/tools/list_changed.

        The default value is false.
    */
    Q_PROPERTY(bool toolsListChanged READ toolsListChanged WRITE setToolsListChanged)

    /*!
        \property QMcpSubscriptionFilter::promptsListChanged
        \brief If true, receive notifications/prompts/list_changed.

        The default value is false.
    */
    Q_PROPERTY(bool promptsListChanged READ promptsListChanged WRITE setPromptsListChanged)

    /*!
        \property QMcpSubscriptionFilter::resourcesListChanged
        \brief If true, receive notifications/resources/list_changed.

        The default value is false.
    */
    Q_PROPERTY(bool resourcesListChanged READ resourcesListChanged WRITE setResourcesListChanged)

    /*!
        \property QMcpSubscriptionFilter::resourceSubscriptions
        \brief Subscribe to notifications/resources/updated for these resource URIs.

        Replaces the former resources/subscribe RPC. The schema types the items
        as plain strings, so they are kept as QString rather than QUrl to round
        trip any URI the server hands out verbatim.
    */
    Q_PROPERTY(QList<QString> resourceSubscriptions READ resourceSubscriptions WRITE setResourceSubscriptions)

public:
    QMcpSubscriptionFilter() : QMcpGadget(new Private) {}

    bool toolsListChanged() const {
        return d<Private>()->toolsListChanged;
    }

    void setToolsListChanged(bool toolsListChanged) {
        if (this->toolsListChanged() == toolsListChanged) return;
        d<Private>()->toolsListChanged = toolsListChanged;
    }

    bool promptsListChanged() const {
        return d<Private>()->promptsListChanged;
    }

    void setPromptsListChanged(bool promptsListChanged) {
        if (this->promptsListChanged() == promptsListChanged) return;
        d<Private>()->promptsListChanged = promptsListChanged;
    }

    bool resourcesListChanged() const {
        return d<Private>()->resourcesListChanged;
    }

    void setResourcesListChanged(bool resourcesListChanged) {
        if (this->resourcesListChanged() == resourcesListChanged) return;
        d<Private>()->resourcesListChanged = resourcesListChanged;
    }

    QList<QString> resourceSubscriptions() const {
        return d<Private>()->resourceSubscriptions;
    }

    void setResourceSubscriptions(const QList<QString> &resourceSubscriptions) {
        if (this->resourceSubscriptions() == resourceSubscriptions) return;
        d<Private>()->resourceSubscriptions = resourceSubscriptions;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        bool toolsListChanged = false;
        bool promptsListChanged = false;
        bool resourcesListChanged = false;
        QList<QString> resourceSubscriptions;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpSubscriptionFilter)

QT_END_NAMESPACE

#endif // QMCPSUBSCRIPTIONFILTER_H
