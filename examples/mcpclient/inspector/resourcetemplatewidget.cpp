// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "resourcetemplatewidget.h"
#include "ui_resourcetemplatewidget.h"

class ResourceTemplateWidget::Private : public Ui::ResourceTemplateWidget
{
public:
    Private(::ResourceTemplateWidget *parent);

private:
    ::ResourceTemplateWidget *q;
public:
    QMcpResourceTemplate resourceTemplate;
};

ResourceTemplateWidget::Private::Private(::ResourceTemplateWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(q, &::ResourceTemplateWidget::resourceTemplateChanged, q, [this](const QMcpResourceTemplate &resourceTemplate) {
        name->setText(resourceTemplate.name());
        mimeType->setText(resourceTemplate.mimeType());
        uriTemplate->setText(resourceTemplate.uriTemplate());
        description->setText(resourceTemplate.description());
    });
}

ResourceTemplateWidget::ResourceTemplateWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

ResourceTemplateWidget::~ResourceTemplateWidget() = default;

QMcpResourceTemplate ResourceTemplateWidget::resourceTemplate() const
{
    return d->resourceTemplate;
}

void ResourceTemplateWidget::setResourceTemplate(const QMcpResourceTemplate &resourceTemplate)
{
    if (d->resourceTemplate == resourceTemplate) return;
    d->resourceTemplate = resourceTemplate;
    emit resourceTemplateChanged(d->resourceTemplate);
}
