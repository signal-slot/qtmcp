// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void setBusy(bool busy);
    void showErrorMessage(const QString &message);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // MAINWINDOW_H
