// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef LISTRESOURCETEMPLATESWIDGET_H
#define LISTRESOURCETEMPLATESWIDGET_H

#include "abstractwidget.h"
#include <QtMcpCommon/QMcpResourceTemplate>

class ListResourceTemplatesWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QList<QMcpResourceTemplate> resourceTemplates READ resourceTemplates NOTIFY resourceTemplatesChanged)
public:
    explicit ListResourceTemplatesWidget(QWidget *parent = nullptr);
    ~ListResourceTemplatesWidget() override;

    QList<QMcpResourceTemplate> resourceTemplates() const;

signals:
    void resourceTemplatesChanged(const QList<QMcpResourceTemplate> &resourceTemplates);

private slots:
    void listResourceTemplates(const QString &nextCursor = QString());

private:
    class Private;
    QScopedPointer<Private> d;
};
#endif // LISTRESOURCETEMPLATESWIDGET_H
