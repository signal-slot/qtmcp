// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LISTRESOURCESWIDGET_H
#define LISTRESOURCESWIDGET_H

#include "abstractwidget.h"
#include <QtMcpCommon/QMcpResource>

class ListResourcesWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QList<QMcpResource> resources READ resources NOTIFY resourcesChanged)
public:
    explicit ListResourcesWidget(QWidget *parent = nullptr);
    ~ListResourcesWidget() override;

    QList<QMcpResource> resources() const;

signals:
    void resourcesChanged(const QList<QMcpResource> &resources);

private slots:
    void listResources(const QString &nextCursor = QString());

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // LISTRESOURCESWIDGET_H
