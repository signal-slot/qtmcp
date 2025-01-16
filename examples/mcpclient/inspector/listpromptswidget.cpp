// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "listpromptswidget.h"
#include "ui_listpromptswidget.h"

#include <QtMcpCommon/QMcpListPromptsRequest>
#include <QtMcpCommon/QMcpListPromptsResult>

class ListPromptsWidget::Private : public Ui::ListPromptsWidget
{
public:
    Private(::ListPromptsWidget *parent);

private:
    ::ListPromptsWidget *q;
public:
    QList<QMcpPrompt> prompts;
};

ListPromptsWidget::Private::Private(::ListPromptsWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(list, &QPushButton::clicked, q, [this]() {
        list->setEnabled(false);
        q->setLoading(true);
        prompts.clear();
        emit q->promptsChanged(prompts);

        QMcpListPromptsRequest request;
        q->client()->request(request, [this](const QMcpListPromptsResult &result, const QMcpJSONRPCErrorError *error) {
            if (error) {
                emit q->errorOccurred(error->message());
                q->setLoading(false);
                list->setEnabled(true);
                return;
            }
            prompts.append(result.prompts());
            qDebug() << result.nextCursor();
            emit q->promptsChanged(prompts);
            q->setLoading(false);
            list->setEnabled(true);
        });
    });
}

ListPromptsWidget::ListPromptsWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

ListPromptsWidget::~ListPromptsWidget() = default;

QList<QMcpPrompt> ListPromptsWidget::prompts() const
{
    return d->prompts;
}
