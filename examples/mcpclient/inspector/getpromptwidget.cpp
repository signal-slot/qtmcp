// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "getpromptwidget.h"
#include "ui_getpromptwidget.h"

#include <QtMcpCommon/QMcpGetPromptRequest>
#include <QtMcpCommon/QMcpGetPromptResult>
#include <QtWidgets/QLineEdit>

class GetPromptWidget::Private : public Ui::GetPromptWidget
{
public:
    Private(::GetPromptWidget *parent);

private:
    ::GetPromptWidget *q;
public:
    QMcpPrompt prompt;
};

GetPromptWidget::Private::Private(::GetPromptWidget *parent)
    : q(parent)
{
    setupUi(q);
    getPrompt->setEnabled(false);

    connect(q, &::GetPromptWidget::promptChanged, q, [this](const QMcpPrompt &prompt) {
        auto paramsLayout = qobject_cast<QFormLayout *>(params->layout());
        while (paramsLayout->rowCount() > 0)
            paramsLayout->removeRow(0);
        auto promptsLayout = qobject_cast<QFormLayout *>(prompts->layout());
        while (promptsLayout->rowCount() > 0)
            promptsLayout->removeRow(0);

        name->setText(prompt.name());
        description->setText(prompt.description());
        const auto arguments = prompt.arguments();
        for (const auto &argument : arguments) {
            auto title = argument.name();
            if (argument.required())
                title += "*"_L1;
            auto lineEdit = new QLineEdit;
            lineEdit->setObjectName(argument.name());
            paramsLayout->addRow(title, lineEdit);
        }
        getPrompt->setEnabled(true);
    });

    connect(getPrompt, &QPushButton::clicked, q, [this]() {
        auto promptLayout = qobject_cast<QFormLayout *>(prompts->layout());
        while (promptLayout->rowCount() > 0)
            promptLayout->removeRow(0);

        QMcpGetPromptRequest request;
        auto params = request.params();
        auto arguments = params.arguments();
        const auto promptArguments = prompt.arguments();
        for (const auto &promptArgument : promptArguments) {
            const auto name = promptArgument.name();
            const auto lineEdit = this->params->findChild<QLineEdit *>(name);
            const auto value = lineEdit->text();
            if (promptArgument.required()) {
                if (value.isEmpty()) {
                    qWarning() << name << "is required";
                    return;
                } else {
                    arguments.insert(name, value);
                }
            } else if (!value.isEmpty()) {
                arguments.insert(name, value);
            }
        }
        params.setArguments(arguments);
        params.setName(prompt.name());
        request.setParams(params);

        getPrompt->setEnabled(false);
        q->setLoading(true);
        q->client()->request(request, [this](const QMcpGetPromptResult &result, const QMcpJSONRPCErrorError *error) {
            if (error) {
                emit q->errorOccurred(error->message());
                q->setLoading(false);
                getPrompt->setEnabled(true);
                return;
            }
            const auto messages = result.messages();
            qDebug() << messages;
            auto promptLayout = qobject_cast<QFormLayout *>(prompts->layout());
            for (const auto &message : messages) {
                const auto content = message.content();
                if (content.refType() == "textContent"_L1) {
                    const auto textContent = content.textContent();
                    const auto text = textContent.text();
                    auto label = new QLabel(text);
                    promptLayout->addRow(textContent.type() + ":", label);
                    // }
                } else if (content.refType() == "imageContent"_L1) {
                    const auto imageContent = content.imageContent();
                    promptLayout->addRow(imageContent.mimeType() + ":", new QWidget);
                } else if (content.refType() == "embeddedResource"_L1) {
                    const auto embeddedResource = content.embeddedResource();
                    promptLayout->addRow(embeddedResource.type() + ":", new QWidget);
                } else {
                    qWarning() << content.refType() << "not supported";
                }
            }
            q->setLoading(false);
            getPrompt->setEnabled(true);
        });
    });
}

GetPromptWidget::GetPromptWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

GetPromptWidget::~GetPromptWidget() = default;

QMcpPrompt GetPromptWidget::prompt() const
{
    return d->prompt;
}

void GetPromptWidget::setPrompt(const QMcpPrompt &prompt)
{
    if (d->prompt == prompt) return;
    d->prompt = prompt;
    emit promptChanged(prompt);
}
