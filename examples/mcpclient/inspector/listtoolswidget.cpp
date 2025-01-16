// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "listtoolswidget.h"
#include "ui_listtoolswidget.h"

#include <QtMcpCommon/QMcpListToolsRequest>
#include <QtMcpCommon/QMcpListToolsResult>

class ListToolsWidget::Private : public Ui::ListToolsWidget
{
public:
    Private(::ListToolsWidget *parent);

private:
    ::ListToolsWidget *q;
public:
    QList<QMcpTool> tools;
};

ListToolsWidget::Private::Private(::ListToolsWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(list, &QPushButton::clicked, q, [this]() {
        list->setEnabled(false);
        q->setLoading(true);
        tools.clear();
        emit q->toolsChanged(tools);

        QMcpListToolsRequest request;
        q->client()->request(request, [this](const QMcpListToolsResult &result, const QMcpJSONRPCErrorError *error) {
            if (error) {
                emit q->errorOccurred(error->message());
                q->setLoading(false);
                list->setEnabled(true);
                return;
            }
            tools.append(result.tools());
            emit q->toolsChanged(tools);
            q->setLoading(false);
            list->setEnabled(true);
        });
    });
}

ListToolsWidget::ListToolsWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

ListToolsWidget::~ListToolsWidget() = default;

QList<QMcpTool> ListToolsWidget::tools() const
{
    return d->tools;
}
