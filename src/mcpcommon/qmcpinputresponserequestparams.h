// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPINPUTRESPONSEREQUESTPARAMS_H
#define QMCPINPUTRESPONSEREQUESTPARAMS_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpInputResponseRequestParams
    \inmodule QtMcpCommon
    \brief Parameters a client sends when retrying a request that answered with QMcpInputRequiredResult.

    The client re-sends the original request, carrying the answers it collected
    for the server's QMcpInputRequiredResult::inputRequests plus the opaque
    QMcpInputRequiredResult::requestState it was handed.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpInputResponseRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpInputResponseRequestParams::inputResponses
        \brief A map of client responses to server-initiated requests.

        Keys correspond to the keys of QMcpInputRequiredResult::inputRequests;
        values are the client's result for each request. The schema types each
        value as an anyOf over CreateMessageResult, ListRootsResult and
        ElicitResult, and the keys are server-assigned, so this stays a raw
        QJsonObject for the same reason as
        QMcpInputRequiredResult::inputRequests: a free-form map keyed by
        server-chosen identifiers holding a union of three result types has no
        faithful gadget representation.
    */
    Q_PROPERTY(QJsonObject inputResponses READ inputResponses WRITE setInputResponses)

    /*!
        \property QMcpInputResponseRequestParams::requestState
        \brief The opaque token received in QMcpInputRequiredResult::requestState.
    */
    Q_PROPERTY(QString requestState READ requestState WRITE setRequestState)

public:
    QMcpInputResponseRequestParams() : QMcpGadget(new Private) {}

    QJsonObject inputResponses() const {
        return d<Private>()->inputResponses;
    }

    void setInputResponses(const QJsonObject &inputResponses) {
        if (this->inputResponses() == inputResponses) return;
        d<Private>()->inputResponses = inputResponses;
    }

    QString requestState() const {
        return d<Private>()->requestState;
    }

    void setRequestState(const QString &requestState) {
        if (this->requestState() == requestState) return;
        d<Private>()->requestState = requestState;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject inputResponses;
        QString requestState;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpInputResponseRequestParams)

QT_END_NAMESPACE

#endif // QMCPINPUTRESPONSEREQUESTPARAMS_H
