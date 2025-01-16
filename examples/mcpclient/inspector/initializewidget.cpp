// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "initializewidget.h"
#include "ui_initializewidget.h"

#include <QtMcpCommon/QMcpInitializeRequest>
#include <QtMcpCommon/QMcpInitializeResult>
#include <QtMcpCommon/QMcpInitializedNotification>

class InitializeWidget::Private : public Ui::InitializeWidget
{
public:
    Private(::InitializeWidget *parent);

private:
    ::InitializeWidget *q;
};

InitializeWidget::Private::Private(::InitializeWidget *parent)
    : q(parent)
{
    setupUi(q);

    const auto settings = q->settings();
    clientName->setText(settings->value("clientName").toString());
    clientVersion->setText(settings->value("clientVersion").toString());

    connect(send, &QPushButton::clicked, q, [this]() {
        if (clientName->text().isEmpty()) return;
        send->setEnabled(false);
        q->setLoading(true);

        QMcpInitializeRequest request;
        auto params = request.params();
        auto clientInfo = params.clientInfo();
        clientInfo.setName(clientName->text());
        clientInfo.setVersion(clientVersion->text());
        params.setClientInfo(clientInfo);
        params.setProtocolVersion("2024-11-05"_L1);
        request.setParams(params);

        auto settings = q->settings();
        settings->setValue("clientName", clientName->text());
        settings->setValue("clientVersion", clientVersion->text());

        q->client()->request(request, [this](const QMcpInitializeResult &result, const QMcpJSONRPCErrorError *) {
            serverName->setText(result.serverInfo().name());
            serverVersion->setText(result.serverInfo().version());
            serverProtocolVersion->setText(result.protocolVersion());
            serverInstructions->setPlainText(result.instructions());
            QMcpInitializedNotification notification;
            q->client()->notify(notification);
            q->setLoading(false);
            send->setEnabled(true);
            emit q->initialized();
        });
    });
}

InitializeWidget::InitializeWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

InitializeWidget::~InitializeWidget() = default;
