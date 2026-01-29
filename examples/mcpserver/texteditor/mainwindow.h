// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QFuture>
#include <QtWidgets/QMainWindow>
#include <QtMcpCommon/QMcpCallToolResultContent>

QT_BEGIN_NAMESPACE
class QTextEdit;
class QMcpServer;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    // MCP Tools (sync)
    Q_INVOKABLE QStringList listFiles(const QString &directory) const;
    Q_INVOKABLE bool openFile(const QString &filePath);
    Q_INVOKABLE bool saveFileAs(const QString &filePath);

    // MCP Tools (async with progress)
    Q_INVOKABLE QFuture<QList<QMcpCallToolResultContent>> toUpperCase();
    Q_INVOKABLE QFuture<QList<QMcpCallToolResultContent>> toLowerCase();

signals:
    void finished(const QString &operation);

public slots:
    void open();
    void saveAs();
    void convertToUpperCase();
    void convertToLowerCase();

private:
    void setupMenus();
    void setupMcpServer();
    QFuture<QList<QMcpCallToolResultContent>> transformText(bool toUpper);

    QTextEdit *m_textEdit = nullptr;
    QMcpServer *m_server = nullptr;
    QString m_currentFilePath;
};

#endif // MAINWINDOW_H
