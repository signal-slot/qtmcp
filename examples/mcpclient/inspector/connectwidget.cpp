// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "connectwidget.h"
#include "ui_connectwidget.h"

class ConnectWidget::Private : public Ui::ConnectWidget
{
public:
    Private(::ConnectWidget *parent);

private:
    ::ConnectWidget *q;
};

ConnectWidget::Private::Private(::ConnectWidget *parent)
    : q(parent)
{
    setupUi(q);

    const auto settings = q->settings();

    backend->addItems(QMcpClient::backends());
    backend->setCurrentText(settings->value("backend", backend->itemText(0)).toString());
    serverName->setCurrentText(settings->value("serverName").toString());

    disconnectFromClient->setEnabled(false);
    connect(connectToServer, &QPushButton::clicked, q, [this]() {
        if (serverName->currentText().isEmpty()) return;
        auto settings = q->settings();
        settings->setValue("backend", backend->currentText());
        settings->setValue("serverName", serverName->currentText());

        connectToServer->setEnabled(false);
        q->setLoading(true);
        auto client = new QMcpClient(backend->currentText(), q);
        client->start(serverName->currentText());
        q->setLoading(false);
        q->setClient(client);
        disconnectFromClient->setEnabled(true);
        // connect(client, &QMcpClient::stopped, ...)
    });
    connect(disconnectFromClient, &QPushButton::clicked, q, [this]() {
        disconnectFromClient->setEnabled(false);
        q->client()->deleteLater();
        q->setClient(nullptr);
        connectToServer->setEnabled(true);
    });
}

ConnectWidget::ConnectWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

ConnectWidget::~ConnectWidget() = default;

