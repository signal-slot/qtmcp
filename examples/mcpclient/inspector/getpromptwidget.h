// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef GETPROMPTWIDGET_H
#define GETPROMPTWIDGET_H

#include "abstractwidget.h"
#include <QtMcpCommon/QMcpPrompt>

class GetPromptWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QMcpPrompt prompt READ prompt WRITE setPrompt NOTIFY promptChanged FINAL)
public:
    explicit GetPromptWidget(QWidget *parent = nullptr);
    ~GetPromptWidget() override;

    QMcpPrompt prompt() const;

public slots:
    void setPrompt(const QMcpPrompt &prompt);

signals:
    void promptChanged(const QMcpPrompt &prompt);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // GETPROMPTWIDGET_H
