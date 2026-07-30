// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPINPUTREQUIREDRESULT_H
#define QMCPINPUTREQUIREDRESULT_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpInputRequiredResult
    \inmodule QtMcpCommon
    \brief Sent by the server to indicate that additional input is needed before the request can be completed.

    This is the interim result of a Multi Round-Trip Request (MRTR): instead of
    completing, the server answers with resultType \c "input_required" and asks
    the client to fulfill the requests in \l inputRequests. The client then
    retries the original request, passing the collected answers back through
    QMcpInputResponseRequestParams.

    At least one of \l inputRequests or \l requestState must be present. The
    schema marks neither as required, so both are optional here and the caller
    is responsible for honoring that constraint.

    \since MCP 2026-07-28
*/
class Q_MCPCOMMON_EXPORT QMcpInputRequiredResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpInputRequiredResult::inputRequests
        \brief A map of server-initiated requests that the client must fulfill.

        Keys are server-assigned identifiers; values are the request objects.
        The schema types each value as an anyOf over CreateMessageRequest,
        ListRootsRequest and ElicitRequest, and the key set is free-form (the
        server picks it per response), so this is kept as a raw QJsonObject
        instead of a typed map: a map with server-defined keys holding a union
        of three request types has no faithful gadget representation. Callers
        pick the variant apart with QMcpCreateMessageRequest,
        QMcpListRootsRequest or QMcpElicitRequest themselves.
    */
    Q_PROPERTY(QJsonObject inputRequests READ inputRequests WRITE setInputRequests)

    /*!
        \property QMcpInputRequiredResult::requestState
        \brief An opaque token the client must echo back when retrying the request.

        The server uses it to resume the interrupted request; clients must treat
        the value as opaque.
    */
    Q_PROPERTY(QString requestState READ requestState WRITE setRequestState)

public:
    QMcpInputRequiredResult() : QMcpResult(new Private) {
        setResultType("input_required"_L1);
    }

    QJsonObject inputRequests() const {
        return d<Private>()->inputRequests;
    }

    void setInputRequests(const QJsonObject &inputRequests) {
        if (this->inputRequests() == inputRequests) return;
        d<Private>()->inputRequests = inputRequests;
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
    struct Private : public QMcpResult::Private {
        QJsonObject inputRequests;
        QString requestState;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpInputRequiredResult)

QT_END_NAMESPACE

#endif // QMCPINPUTREQUIREDRESULT_H
