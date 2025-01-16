// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef ABSTRACTWIDGET_H
#define ABSTRACTWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCore/QSettings>
#include <QtMcpClient/QMcpClient>

class AbstractWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)
    Q_PROPERTY(QMcpClient *client READ client WRITE setClient NOTIFY clientChanged FINAL)
public:
    explicit AbstractWidget(QWidget *parent = nullptr);
    ~AbstractWidget() override;

    bool isLoading() const;
    QMcpClient *client() const;

protected slots:
    void setLoading(bool loading);
public:
    void setClient(QMcpClient *client);

signals:
    void loadingChanged(bool loading);
    void clientChanged(QMcpClient *client);
    void errorOccurred(const QString &message);

protected:
    QSettings *settings() const;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // ABSTRACTWIDGET_H
