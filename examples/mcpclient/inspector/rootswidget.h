// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef ROOTSWIDGET_H
#define ROOTSWIDGET_H

#include "abstractwidget.h"

class RootsWidget : public AbstractWidget
{
    Q_OBJECT
public:
    explicit RootsWidget(QWidget *parent = nullptr);
    ~RootsWidget() override;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // ROOTSWIDGET_H
