// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "listresourceswidget.h"
#include "ui_listresourceswidget.h"

#include <QtMcpCommon/QMcpListResourcesRequest>
#include <QtMcpCommon/QMcpListResourcesResult>

class ListResourcesWidget::Private : public Ui::ListResourcesWidget
{
public:
    Private(::ListResourcesWidget *parent);

private:
    ::ListResourcesWidget *q;
public:
    QList<QMcpResource> resources;
};

ListResourcesWidget::Private::Private(::ListResourcesWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(list, &QPushButton::clicked, q, [this]() {
        list->setEnabled(false);
        q->setLoading(true);
        resources.clear();
        emit q->resourcesChanged(resources);
        q->listResources();
    });
}

ListResourcesWidget::ListResourcesWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

ListResourcesWidget::~ListResourcesWidget() = default;

QList<QMcpResource> ListResourcesWidget::resources() const
{
    return d->resources;
}

void ListResourcesWidget::listResources(const QString &nextCursor) {
    QMcpListResourcesRequest request;
    auto params = request.params();
    params.setCursor(nextCursor);
    request.setParams(params);
    client()->request(request, [&](const QMcpListResourcesResult &result, const QMcpJSONRPCErrorError *error) {
        if (error) {
            emit errorOccurred(error->message());
            setLoading(false);
            d->list->setEnabled(true);
            return;
        }
        const auto resources = result.resources();
        d->resources.append(resources);
        emit resourcesChanged(d->resources);

        if (result.nextCursor().isEmpty()) {
            setLoading(false);
            d->list->setEnabled(true);
        } else {
            listResources(result.nextCursor());
        }
    });
}
