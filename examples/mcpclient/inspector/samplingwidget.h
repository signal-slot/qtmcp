// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SAMPLINGWIDGET_H
#define SAMPLINGWIDGET_H

#include "abstractwidget.h"

class SamplingWidget : public AbstractWidget
{
    Q_OBJECT
public:
    explicit SamplingWidget(QWidget *parent = nullptr);
    ~SamplingWidget() override;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // SAMPLINGWIDGET_H
