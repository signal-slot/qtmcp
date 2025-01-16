// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LISTTOOLSWIDGET_H
#define LISTTOOLSWIDGET_H

#include "abstractwidget.h"
#include <QtMcpCommon/QMcpTool>

class ListToolsWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QList<QMcpTool> tools READ tools NOTIFY toolsChanged)
public:
    explicit ListToolsWidget(QWidget *parent = nullptr);
    ~ListToolsWidget() override;

    QList<QMcpTool> tools() const;

signals:
    void toolsChanged(const QList<QMcpTool> &tools);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // LISTTOOLSWIDGET_H
