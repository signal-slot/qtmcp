// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PINGWIDGET_H
#define PINGWIDGET_H

#include "abstractwidget.h"

class PingWidget : public AbstractWidget
{
    Q_OBJECT
public:
    explicit PingWidget(QWidget *parent = nullptr);
    ~PingWidget() override;

private:
    class Private;
    QScopedPointer<Private> d;
};


#endif // PINGWIDGET_H
