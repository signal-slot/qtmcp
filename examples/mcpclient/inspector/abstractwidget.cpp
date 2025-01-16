// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "abstractwidget.h"

class AbstractWidget::Private
{
public:
    bool loading = false;
    QMcpClient *client = nullptr;
    QScopedPointer<QSettings> settings;
};

AbstractWidget::AbstractWidget(QWidget *parent)
    : QWidget{parent}
    , d(new Private)
{}

AbstractWidget::~AbstractWidget() = default;

bool AbstractWidget::isLoading() const
{
    return d->loading;
}

void AbstractWidget::setLoading(bool loading)
{
    if (d->loading == loading) return;
    d->loading = loading;
    emit loadingChanged(loading);
}

QMcpClient *AbstractWidget::client() const
{
    return d->client;
}

void AbstractWidget::setClient(QMcpClient *client)
{
    if (d->client == client) return;
    d->client = client;
    emit clientChanged(client);
}

QSettings *AbstractWidget::settings() const
{
    if (!d->settings) {
        d->settings.reset(new QSettings);
        d->settings->beginGroup(metaObject()->className());
    }
    return d->settings.data();
}
