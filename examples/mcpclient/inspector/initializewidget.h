// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef INITIALIZEWIDGET_H
#define INITIALIZEWIDGET_H

#include "abstractwidget.h"

class InitializeWidget : public AbstractWidget
{
    Q_OBJECT
public:
    explicit InitializeWidget(QWidget *parent = nullptr);
    ~InitializeWidget() override;

signals:
    void initialized();

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // INITIALIZEWIDGET_H
