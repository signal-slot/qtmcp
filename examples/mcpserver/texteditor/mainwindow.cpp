// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "mainwindow.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QPromise>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTextEdit>
#include <QtMcpCommon/QMcpTextContent>
#include <QtMcpServer/QMcpServer>

using namespace Qt::Literals::StringLiterals;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_textEdit(new QTextEdit(this))
{
    setCentralWidget(m_textEdit);
    setWindowTitle(tr("Text Editor - MCP Server"));
    resize(800, 600);

    setupMenus();
    setupMcpServer();
}

void MainWindow::setupMenus()
{
    auto *fileMenu = menuBar()->addMenu(tr("&File"));

    auto *openAction = fileMenu->addAction(tr("&Open..."));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::open);

    auto *saveAsAction = fileMenu->addAction(tr("Save &As..."));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);

    fileMenu->addSeparator();

    auto *exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    auto *editMenu = menuBar()->addMenu(tr("&Edit"));

    auto *upperCaseAction = editMenu->addAction(tr("To &Uppercase"));
    upperCaseAction->setShortcut(QKeySequence(tr("Ctrl+U")));
    connect(upperCaseAction, &QAction::triggered, this, &MainWindow::convertToUpperCase);

    auto *lowerCaseAction = editMenu->addAction(tr("To &Lowercase"));
    lowerCaseAction->setShortcut(QKeySequence(tr("Ctrl+L")));
    connect(lowerCaseAction, &QAction::triggered, this, &MainWindow::convertToLowerCase);
}

void MainWindow::setupMcpServer()
{
    m_server = new QMcpServer(u"stdio"_s, this);
    m_server->registerToolSet(this, {
        { u"listFiles"_s, u"List all .txt files in the specified directory"_s },
        { u"listFiles/directory"_s, u"Directory path to search for .txt files"_s },
        { u"openFile"_s, u"Open a text file in the editor"_s },
        { u"openFile/filePath"_s, u"Full path to the .txt file to open"_s },
        { u"toUpperCase"_s, u"Convert the current document text to uppercase (async with progress)"_s },
        { u"toLowerCase"_s, u"Convert the current document text to lowercase (async with progress)"_s },
        { u"saveFileAs"_s, u"Save the current document to a new file"_s },
        { u"saveFileAs/filePath"_s, u"Full path where to save the file"_s },
    });
    m_server->start();
}

void MainWindow::open()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Text File"),
        QString(),
        tr("Text Files (*.txt);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        openFile(filePath);
    }
}

void MainWindow::saveAs()
{
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Text File"),
        QString(),
        tr("Text Files (*.txt);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        saveFileAs(filePath);
    }
}

void MainWindow::convertToUpperCase()
{
    // For GUI, just do it synchronously
    const QString text = m_textEdit->toPlainText();
    if (!text.isEmpty()) {
        m_textEdit->setPlainText(text.toUpper());
    }
}

void MainWindow::convertToLowerCase()
{
    // For GUI, just do it synchronously
    const QString text = m_textEdit->toPlainText();
    if (!text.isEmpty()) {
        m_textEdit->setPlainText(text.toLower());
    }
}

QStringList MainWindow::listFiles(const QString &directory) const
{
    QDir dir(directory);
    if (!dir.exists()) {
        return {};
    }

    QStringList filters;
    filters << u"*.txt"_s;
    dir.setNameFilters(filters);

    QStringList result;
    const auto entries = dir.entryInfoList(QDir::Files | QDir::Readable);
    for (const auto &entry : entries) {
        result << entry.absoluteFilePath();
    }
    return result;
}

bool MainWindow::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot open file: %1").arg(filePath));
        return false;
    }

    QTextStream in(&file);
    m_textEdit->setPlainText(in.readAll());
    m_currentFilePath = filePath;
    setWindowTitle(tr("Text Editor - %1").arg(filePath));
    return true;
}

bool MainWindow::saveFileAs(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Cannot save file: %1").arg(filePath));
        return false;
    }

    QTextStream out(&file);
    out << m_textEdit->toPlainText();
    m_currentFilePath = filePath;
    setWindowTitle(tr("Text Editor - %1").arg(filePath));
    return true;
}

QFuture<QList<QMcpCallToolResultContent>> MainWindow::transformText(bool toUpper)
{
    const QString text = m_textEdit->toPlainText();

    // Run in a separate thread with progress reporting
    return QtConcurrent::run([this, text, toUpper]() {
        QList<QMcpCallToolResultContent> result;

        if (text.isEmpty()) {
            QMcpTextContent content;
            content.setText(u"No text to transform"_s);
            result.append(content);
            return result;
        }

        // Process text character by character with progress
        QString transformed;
        transformed.reserve(text.size());

        const int totalChars = text.size();

        for (int i = 0; i < totalChars; ++i) {
            QChar ch = text.at(i);
            transformed.append(toUpper ? ch.toUpper() : ch.toLower());

            // Simulate slower processing for demo (every 100 chars, sleep 10ms)
            if (i % 100 == 0) {
                QThread::msleep(10);
            }
        }

        // Update the text edit on the main thread
        QMetaObject::invokeMethod(m_textEdit, [this, transformed]() {
            m_textEdit->setPlainText(transformed);
        }, Qt::QueuedConnection);

        QMcpTextContent content;
        content.setText(toUpper ? u"Text converted to uppercase"_s : u"Text converted to lowercase"_s);
        result.append(content);
        return result;
    });
}

QFuture<QList<QMcpCallToolResultContent>> MainWindow::toUpperCase()
{
    return transformText(true);
}

QFuture<QList<QMcpCallToolResultContent>> MainWindow::toLowerCase()
{
    return transformText(false);
}
