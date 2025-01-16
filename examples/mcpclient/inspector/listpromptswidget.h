// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LISTPROMPTSWIDGET_H
#define LISTPROMPTSWIDGET_H

#include "abstractwidget.h"

#include <QtMcpCommon/QMcpPrompt>

class ListPromptsWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QList<QMcpPrompt> prompts READ prompts NOTIFY promptsChanged)
public:
    explicit ListPromptsWidget(QWidget *parent = nullptr);
    ~ListPromptsWidget() override;

    QList<QMcpPrompt> prompts() const;

signals:
    void promptsChanged(const QList<QMcpPrompt> &prompts);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // LISTPROMPTSWIDGET_H
