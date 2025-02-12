// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "samplingwidget.h"
#include "ui_samplingwidget.h"

#include <QtCore/QJsonDocument>
#include <QtMcpCommon/QMcpCreateMessageRequest>
#include <QtMcpCommon/QMcpCreateMessageResult>

class SamplingWidget::Private : public Ui::SamplingWidget
{
public:
    Private(::SamplingWidget *parent);

private:
    ::SamplingWidget *q;
};

SamplingWidget::Private::Private(::SamplingWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(q, &AbstractWidget::clientChanged, q, [this](QMcpClient *client) {
        if (client) {
            client->addRequestHandler(
                [this](const QMcpCreateMessageRequest &request, QMcpJSONRPCErrorError *) -> QMcpCreateMessageResult {
                    qDebug() << request;
                    const auto params = request.params();
                    const auto messages = params.messages();
                    for (const auto &message : messages) {
                        plainTextEdit->appendPlainText(QString::fromUtf8(QJsonDocument(QMcpSamplingMessage(message).toJsonObject()).toJson()));
                    }
                    QMcpCreateMessageResult result;
                    auto content = result.content();
                    auto textContent = content.textContent();
                    textContent.setText("Hello");
                    content.setTextContent(textContent);
                    result.setContent(content);
                    return result;
                });
        }
    });
}

SamplingWidget::SamplingWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

SamplingWidget::~SamplingWidget() = default;
