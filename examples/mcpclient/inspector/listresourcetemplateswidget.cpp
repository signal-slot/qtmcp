// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "listresourcetemplateswidget.h"
#include "ui_listresourcetemplateswidget.h"

#include <QtMcpCommon/QMcpListResourceTemplatesRequest>
#include <QtMcpCommon/QMcpListResourceTemplatesResult>

class ListResourceTemplatesWidget::Private : public Ui::ListResourceTemplatesWidget
{
public:
    Private(::ListResourceTemplatesWidget *parent);

private:
    ::ListResourceTemplatesWidget *q;
public:
    QList<QMcpResourceTemplate> resourceTemplates;
};

ListResourceTemplatesWidget::Private::Private(::ListResourceTemplatesWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(list, &QPushButton::clicked, q, [this]() {
        list->setEnabled(false);
        q->setLoading(true);
        resourceTemplates.clear();
        emit q->resourceTemplatesChanged(resourceTemplates);
        q->listResourceTemplates();
    });
}

ListResourceTemplatesWidget::ListResourceTemplatesWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

ListResourceTemplatesWidget::~ListResourceTemplatesWidget() = default;

QList<QMcpResourceTemplate> ListResourceTemplatesWidget::resourceTemplates() const
{
    return d->resourceTemplates;
}

void ListResourceTemplatesWidget::listResourceTemplates(const QString &nextCursor) {
    QMcpListResourceTemplatesRequest request;
    auto params = request.params();
    params.setCursor(nextCursor);
    request.setParams(params);
    client()->request(request, [&](const QMcpListResourceTemplatesResult &result, const QMcpJSONRPCErrorError *error) {
        if (error) {
            emit errorOccurred(error->message());
            setLoading(false);
            d->list->setEnabled(true);
            return;
        }
        const auto resourceTemplates = result.resourceTemplates();
        d->resourceTemplates.append(resourceTemplates);
        emit resourceTemplatesChanged(d->resourceTemplates);

        if (result.nextCursor().isEmpty()) {
            setLoading(false);
            d->list->setEnabled(true);
        } else {
            listResourceTemplates(result.nextCursor());
        }
    });
}
