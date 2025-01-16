// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "pingwidget.h"
#include "ui_pingwidget.h"

#include <QtMcpCommon/QMcpPingRequest>
#include <QtMcpCommon/QMcpResult>

class PingWidget::Private : public Ui::PingWidget
{
public:
    Private(::PingWidget *parent);

private:
    ::PingWidget *q;
};

PingWidget::Private::Private(::PingWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(ping, &QPushButton::clicked, q, [this]() {
        ping->setEnabled(false);
        q->setLoading(true);
        QMcpPingRequest request;
        q->client()->request(request, [this](const QMcpResult &result, const QMcpJSONRPCErrorError *) {
            Q_UNUSED(result);
            q->setLoading(false);
            ping->setEnabled(true);
        });
    });
}

PingWidget::PingWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

PingWidget::~PingWidget() = default;
