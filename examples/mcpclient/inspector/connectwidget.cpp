// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "connectwidget.h"
#include "ui_connectwidget.h"

#include <QtCore/QStringList>

class ConnectWidget::Private : public Ui::ConnectWidget
{
public:
    Private(::ConnectWidget *parent);

private:
    void loadServerHistory(const QString &backendName);
    void saveServerToHistory(const QString &backendName, const QString &serverName);

private:
    ::ConnectWidget *q;
    static const int MaxHistoryItems = 10;
};

void ConnectWidget::Private::loadServerHistory(const QString &backendName)
{
    const auto settings = q->settings();
    serverName->clear();
    const QStringList history = settings->value("history/%1"_L1.arg(backendName)).toStringList();
    serverName->addItems(history);
    
    // Add default item if history is empty
    if (history.isEmpty()) {
        if (backendName == "stdio")
            serverName->addItem("npx @modelcontextprotocol/server-everything");
        if (backendName == "sse")
            serverName->addItem("http://localhost:8080");
    }
}

void ConnectWidget::Private::saveServerToHistory(const QString &backendName, const QString &server)
{
    if (server.isEmpty())
        return;

    const auto settings = q->settings();
    const QString historyKey = "history/%1"_L1.arg(backendName);
    QStringList history = settings->value(historyKey).toStringList();
    
    // Remove if exists and prepend to make it first
    history.removeAll(server);
    history.prepend(server);
    
    // Keep only MaxHistoryItems
    while (history.size() > MaxHistoryItems)
        history.removeLast();
        
    settings->setValue(historyKey, history);
}

ConnectWidget::Private::Private(::ConnectWidget *parent)
    : q(parent)
{
    setupUi(q);

    const auto settings = q->settings();

    backend->addItems(QMcpClient::backends());
    const QString defaultBackend = backend->itemText(0);
    const QString savedBackend = settings->value("backend", defaultBackend).toString();
    backend->setCurrentText(savedBackend);
    
    // Load initial history
    loadServerHistory(savedBackend);
    
    // Update history when backend changes
    connect(backend, &QComboBox::currentTextChanged, q, [this](const QString &text) {
        loadServerHistory(text);
    });

    disconnectFromClient->setEnabled(false);
    connect(connectToServer, &QPushButton::clicked, q, [this]() {
        const QString server = serverName->currentText();
        if (server.isEmpty()) return;

        // For SSE backend, validate URL format
        if (backend->currentText() == "sse") {
            QUrl url(server);
            if (!url.isValid() || !url.scheme().startsWith("http"_L1)) {
                emit q->errorOccurred(tr("Invalid server URL. For SSE backend, please provide a complete URL (e.g., http://localhost:8000)"));
                return;
            }
        }

        auto settings = q->settings();
        settings->setValue("backend", backend->currentText());
        settings->setValue("serverName", server);
        
        // Save to history
        saveServerToHistory(backend->currentText(), server);

        connectToServer->setEnabled(false);
        q->setLoading(true);
        
        auto client = new QMcpClient(backend->currentText(), q);
        
        // Connect to error signal before starting
        connect(client, &QMcpClient::errorOccurred, q, [this, client](const QString &error) {
            qDebug() << "Error occurred:" << error;
            q->setLoading(false);
            emit q->errorOccurred(error);
            client->deleteLater();
            connectToServer->setEnabled(true);
        });
        
        // Connect to started signal to update UI only after successful connection
        connect(client, &QMcpClient::started, q, [this, client]() {
            q->setLoading(false);
            q->setClient(client);
            disconnectFromClient->setEnabled(true);
        });
        
        client->start(server);
    });
    connect(disconnectFromClient, &QPushButton::clicked, q, [this]() {
        disconnectFromClient->setEnabled(false);
        q->client()->deleteLater();
        q->setClient(nullptr);
        connectToServer->setEnabled(true);
    });
}

ConnectWidget::ConnectWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

ConnectWidget::~ConnectWidget() = default;

