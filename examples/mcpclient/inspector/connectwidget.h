// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include "abstractwidget.h"

class ConnectWidget : public AbstractWidget
{
    Q_OBJECT
public:
    explicit ConnectWidget(QWidget *parent = nullptr);
    ~ConnectWidget() override;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // CONNECTWIDGET_H
