// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTREQUEST_H
#define QMCPCLIENTREQUEST_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcpinitializerequest.h>
#include <QtMcpCommon/qmcppingrequest.h>
#include <QtMcpCommon/qmcplistresourcesrequest.h>
#include <QtMcpCommon/qmcplistresourcetemplatesrequest.h>
#include <QtMcpCommon/qmcpreadresourcerequest.h>
#include <QtMcpCommon/qmcpsubscriberequest.h>
#include <QtMcpCommon/qmcpunsubscriberequest.h>
#include <QtMcpCommon/qmcplistpromptsrequest.h>
#include <QtMcpCommon/qmcpgetpromptrequest.h>
#include <QtMcpCommon/qmcplisttoolsrequest.h>
#include <QtMcpCommon/qmcpcalltoolrequest.h>
#include <QtMcpCommon/qmcpsetlevelrequest.h>
#include <QtMcpCommon/qmcpcompleterequest.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpClientRequest : public QMcpAnyOf
{
    Q_GADGET
    Q_PROPERTY(QMcpInitializeRequest initializeRequest READ initializeRequest WRITE setInitializeRequest)
    Q_PROPERTY(QMcpPingRequest pingRequest READ pingRequest WRITE setPingRequest)
    Q_PROPERTY(QMcpListResourcesRequest listResourcesRequest READ listResourcesRequest WRITE setListResourcesRequest)
    Q_PROPERTY(QMcpListResourceTemplatesRequest listResourceTemplatesRequest READ listResourceTemplatesRequest WRITE setListResourceTemplatesRequest)
    Q_PROPERTY(QMcpReadResourceRequest readResourceRequest READ readResourceRequest WRITE setReadResourceRequest)
    Q_PROPERTY(QMcpSubscribeRequest subscribeRequest READ subscribeRequest WRITE setSubscribeRequest)
    Q_PROPERTY(QMcpUnsubscribeRequest unsubscribeRequest READ unsubscribeRequest WRITE setUnsubscribeRequest)
    Q_PROPERTY(QMcpListPromptsRequest listPromptsRequest READ listPromptsRequest WRITE setListPromptsRequest)
    Q_PROPERTY(QMcpGetPromptRequest getPromptRequest READ getPromptRequest WRITE setGetPromptRequest)
    Q_PROPERTY(QMcpListToolsRequest listToolsRequest READ listToolsRequest WRITE setListToolsRequest)
    Q_PROPERTY(QMcpCallToolRequest callToolRequest READ callToolRequest WRITE setCallToolRequest)
    Q_PROPERTY(QMcpSetLevelRequest setLevelRequest READ setLevelRequest WRITE setSetLevelRequest)
    Q_PROPERTY(QMcpCompleteRequest completeRequest READ completeRequest WRITE setCompleteRequest)
public:
    QMcpClientRequest() : QMcpAnyOf(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QMcpInitializeRequest initializeRequest() const {
        return d<Private>()->initializeRequest;
    }

    void setInitializeRequest(const QMcpInitializeRequest &initializeRequest) {
        if (this->initializeRequest() == initializeRequest) return;
        setRefType("initializeRequest"_ba);
        d<Private>()->initializeRequest = initializeRequest;
    }

    QMcpPingRequest pingRequest() const {
        return d<Private>()->pingRequest;
    }

    void setPingRequest(const QMcpPingRequest &pingRequest) {
        if (this->pingRequest() == pingRequest) return;
        setRefType("pingRequest"_ba);
        d<Private>()->pingRequest = pingRequest;
    }

    QMcpListResourcesRequest listResourcesRequest() const {
        return d<Private>()->listResourcesRequest;
    }

    void setListResourcesRequest(const QMcpListResourcesRequest &listResourcesRequest) {
        if (this->listResourcesRequest() == listResourcesRequest) return;
        setRefType("listResourcesRequest"_ba);
        d<Private>()->listResourcesRequest = listResourcesRequest;
    }

    QMcpListResourceTemplatesRequest listResourceTemplatesRequest() const {
        return d<Private>()->listResourceTemplatesRequest;
    }

    void setListResourceTemplatesRequest(const QMcpListResourceTemplatesRequest &listResourceTemplatesRequest) {
        if (this->listResourceTemplatesRequest() == listResourceTemplatesRequest) return;
        setRefType("listResourceTemplatesRequest"_ba);
        d<Private>()->listResourceTemplatesRequest = listResourceTemplatesRequest;
    }

    QMcpReadResourceRequest readResourceRequest() const {
        return d<Private>()->readResourceRequest;
    }

    void setReadResourceRequest(const QMcpReadResourceRequest &readResourceRequest) {
        if (this->readResourceRequest() == readResourceRequest) return;
        setRefType("readResourceRequest"_ba);
        d<Private>()->readResourceRequest = readResourceRequest;
    }

    QMcpSubscribeRequest subscribeRequest() const {
        return d<Private>()->subscribeRequest;
    }

    void setSubscribeRequest(const QMcpSubscribeRequest &subscribeRequest) {
        if (this->subscribeRequest() == subscribeRequest) return;
        setRefType("subscribeRequest"_ba);
        d<Private>()->subscribeRequest = subscribeRequest;
    }

    QMcpUnsubscribeRequest unsubscribeRequest() const {
        return d<Private>()->unsubscribeRequest;
    }

    void setUnsubscribeRequest(const QMcpUnsubscribeRequest &unsubscribeRequest) {
        if (this->unsubscribeRequest() == unsubscribeRequest) return;
        setRefType("unsubscribeRequest"_ba);
        d<Private>()->unsubscribeRequest = unsubscribeRequest;
    }

    QMcpListPromptsRequest listPromptsRequest() const {
        return d<Private>()->listPromptsRequest;
    }

    void setListPromptsRequest(const QMcpListPromptsRequest &listPromptsRequest) {
        if (this->listPromptsRequest() == listPromptsRequest) return;
        setRefType("listPromptsRequest"_ba);
        d<Private>()->listPromptsRequest = listPromptsRequest;
    }

    QMcpGetPromptRequest getPromptRequest() const {
        return d<Private>()->getPromptRequest;
    }

    void setGetPromptRequest(const QMcpGetPromptRequest &getPromptRequest) {
        if (this->getPromptRequest() == getPromptRequest) return;
        setRefType("getPromptRequest"_ba);
        d<Private>()->getPromptRequest = getPromptRequest;
    }

    QMcpListToolsRequest listToolsRequest() const {
        return d<Private>()->listToolsRequest;
    }

    void setListToolsRequest(const QMcpListToolsRequest &listToolsRequest) {
        if (this->listToolsRequest() == listToolsRequest) return;
        setRefType("listToolsRequest"_ba);
        d<Private>()->listToolsRequest = listToolsRequest;
    }

    QMcpCallToolRequest callToolRequest() const {
        return d<Private>()->callToolRequest;
    }

    void setCallToolRequest(const QMcpCallToolRequest &callToolRequest) {
        if (this->callToolRequest() == callToolRequest) return;
        setRefType("callToolRequest"_ba);
        d<Private>()->callToolRequest = callToolRequest;
    }

    QMcpSetLevelRequest setLevelRequest() const {
        return d<Private>()->setLevelRequest;
    }

    void setSetLevelRequest(const QMcpSetLevelRequest &setLevelRequest) {
        if (this->setLevelRequest() == setLevelRequest) return;
        setRefType("setLevelRequest"_ba);
        d<Private>()->setLevelRequest = setLevelRequest;
    }

    QMcpCompleteRequest completeRequest() const {
        return d<Private>()->completeRequest;
    }

    void setCompleteRequest(const QMcpCompleteRequest &completeRequest) {
        if (this->completeRequest() == completeRequest) return;
        setRefType("completeRequest"_ba);
        d<Private>()->completeRequest = completeRequest;
    }

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpInitializeRequest initializeRequest;
        QMcpPingRequest pingRequest;
        QMcpListResourcesRequest listResourcesRequest;
        QMcpListResourceTemplatesRequest listResourceTemplatesRequest;
        QMcpReadResourceRequest readResourceRequest;
        QMcpSubscribeRequest subscribeRequest;
        QMcpUnsubscribeRequest unsubscribeRequest;
        QMcpListPromptsRequest listPromptsRequest;
        QMcpGetPromptRequest getPromptRequest;
        QMcpListToolsRequest listToolsRequest;
        QMcpCallToolRequest callToolRequest;
        QMcpSetLevelRequest setLevelRequest;
        QMcpCompleteRequest completeRequest;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpClientRequest)

QT_END_NAMESPACE

#endif // QMCPCLIENTREQUEST_H
