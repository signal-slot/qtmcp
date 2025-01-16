// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef READRESOURCEWIDGET_H
#define READRESOURCEWIDGET_H

#include "abstractwidget.h"
#include <QtMcpCommon/QMcpResource>

class ReadResourceWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QMcpResource resource READ resource WRITE setResource NOTIFY resourceChanged)
public:
    explicit ReadResourceWidget(QWidget *parent = nullptr);
    ~ReadResourceWidget() override;

    QMcpResource resource() const;

public slots:
    void setResource(const QMcpResource &resource);

signals:
    void resourceChanged(const QMcpResource &resource);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // READRESOURCEWIDGET_H
