// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "readresourcewidget.h"
#include "ui_readresourcewidget.h"

#include <QtMcpCommon/QMcpSubscribeRequest>
#include <QtMcpCommon/QMcpUnsubscribeRequest>
#include <QtMcpCommon/QMcpResourceUpdatedNotification>
#include <QtMcpCommon/QMcpReadResourceRequest>
#include <QtMcpCommon/QMcpReadResourceResult>
#include <QtWidgets/QPlainTextEdit>

class ReadResourceWidget::Private : public Ui::ReadResourceWidget
{
public:
    Private(::ReadResourceWidget *parent);
    void readResource();
    void updateButtons();

private:
    ::ReadResourceWidget *q;
public:
    QMcpResource resource;
private:
    QSet<QUrl> subscribing;
};

ReadResourceWidget::Private::Private(::ReadResourceWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(q, &::ReadResourceWidget::resourceChanged, q, [this](const QMcpResource &resource) {
        name->setText(resource.name());
        url->setText(resource.uri().toString());
        description->setText(resource.description());
        updateButtons();
        auto contentsLayout = qobject_cast<QFormLayout *>(contents->layout());
        while (contentsLayout->rowCount() > 0)
            contentsLayout->removeRow(0);
    });

    connect(q, &AbstractWidget::clientChanged, q, [this](QMcpClient *client) {
        if (client) {
            client->addNotificationHandler(
                [this](const QMcpResourceUpdatedNotification &notification) {
                    if (notification.params().uri() != resource.uri()) return;
                    readResource();
                });
        }
    });

    connect(subscribe, &QPushButton::clicked, q, [this]() {
        q->setLoading(true);
        QMcpSubscribeRequest request;
        auto params = request.params();
        params.setUri(resource.uri());
        request.setParams(params);
        q->client()->request(request);
        subscribing.insert(resource.uri());
        updateButtons();
        q->setLoading(false);

        readResource();
    });

    connect(unsubscribe, &QPushButton::clicked, q, [this]() {
        q->setLoading(true);
        QMcpUnsubscribeRequest request;
        auto params = request.params();
        params.setUri(resource.uri());
        request.setParams(params);
        q->client()->request(request);
        subscribing.remove(resource.uri());
        updateButtons();
        q->setLoading(false);
        readResource();
    });

    connect(read, &QPushButton::clicked, q, [this]() {
        readResource();
    });
}

void ReadResourceWidget::Private::updateButtons()
{
    bool s = subscribing.contains(resource.uri());
    subscribe->setEnabled(!s);
    unsubscribe->setEnabled(s);
    read->setEnabled(!s);
}

void ReadResourceWidget::Private::readResource()
{
    auto contentsLayout = qobject_cast<QFormLayout *>(contents->layout());
    while (contentsLayout->rowCount() > 0)
        contentsLayout->removeRow(0);
    QMcpReadResourceRequest request;
    auto params = request.params();
    params.setUri(resource.uri());
    request.setParams(params);

    read->setEnabled(false);
    q->setLoading(true);
    q->client()->request(request, [this](const QMcpReadResourceResult &result, const QMcpJSONRPCErrorError *error) {
        if (error) {
            emit q->errorOccurred(error->message());
            q->setLoading(false);
            read->setEnabled(true);
            return;
        }
        const auto contents = result.contents();
        qDebug() << contents;
        auto contentsLayout = qobject_cast<QFormLayout *>(this->contents->layout());
        for (const auto &content : contents) {
            if (content.refType() == "textResourceContents"_L1) {
                const auto textResourceContents = content.textResourceContents();
                const auto text = textResourceContents.text();
                if (text.contains("\n"_L1)) {
                    auto textEdit = new QPlainTextEdit(textResourceContents.text());
                    textEdit->setReadOnly(true);
                    contentsLayout->addRow(textResourceContents.mimeType() + ":", textEdit);
                } else {
                    auto label = new QLabel(text);
                    contentsLayout->addRow(textResourceContents.mimeType() + ":", label);
                }
            } else if (content.refType() == "blobResourceContents"_L1) {
                const auto blobResourceContents = content.blobResourceContents();
                contentsLayout->addRow(blobResourceContents.mimeType() + ":", new QWidget);
            } else {
                qWarning() << content.refType() << "not supported";
            }
        }
        q->setLoading(false);
        read->setEnabled(true);
        updateButtons();
    });
}

ReadResourceWidget::ReadResourceWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

ReadResourceWidget::~ReadResourceWidget() = default;

QMcpResource ReadResourceWidget::resource() const
{
    return d->resource;
}

void ReadResourceWidget::setResource(const QMcpResource &resource)
{
    if (d->resource == resource) return;
    d->resource = resource;
    emit resourceChanged(d->resource);
}
