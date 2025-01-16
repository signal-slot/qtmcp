// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef RESOURCETEMPLATEWIDGET_H
#define RESOURCETEMPLATEWIDGET_H

#include "abstractwidget.h"
#include <QtMcpCommon/QMcpResourceTemplate>

class ResourceTemplateWidget : public AbstractWidget
{
    Q_OBJECT
    Q_PROPERTY(QMcpResourceTemplate resourceTemplate READ resourceTemplate WRITE setResourceTemplate NOTIFY resourceTemplateChanged)
public:
    explicit ResourceTemplateWidget(QWidget *parent = nullptr);
    ~ResourceTemplateWidget() override;

    QMcpResourceTemplate resourceTemplate() const;

public slots:
    void setResourceTemplate(const QMcpResourceTemplate &resourceTemplate);

signals:
    void resourceTemplateChanged(const QMcpResourceTemplate &resourceTemplate);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // RESOURCETEMPLATEWIDGET_H
