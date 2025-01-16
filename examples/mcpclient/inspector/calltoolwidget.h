// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef CALLTOOLWIDGET_H
#define CALLTOOLWIDGET_H

#include "abstractwidget.h"
#include <QtMcpCommon/QMcpTool>

class CallToolWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QMcpTool tool READ tool WRITE setTool NOTIFY toolChanged FINAL)
public:
    explicit CallToolWidget(QWidget *parent = nullptr);
    ~CallToolWidget() override;

    QMcpTool tool() const;

public slots:
    void setTool(const QMcpTool &tool);

signals:
    void toolChanged(const QMcpTool &tool);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // CALLTOOLWIDGET_H
