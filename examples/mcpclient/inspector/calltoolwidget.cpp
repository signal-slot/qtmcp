// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "calltoolwidget.h"
#include "ui_calltoolwidget.h"

#include <QtCore/QMimeDatabase>
#include <QtMcpCommon/QMcpCallToolRequest>
#include <QtMcpCommon/QMcpCallToolResult>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QDoubleSpinBox>

class CallToolWidget::Private : public Ui::CallToolWidget
{
public:
    Private(::CallToolWidget *parent);

private:
    ::CallToolWidget *q;
    QMimeDatabase mimeDatabase;
public:
    QMcpTool tool;
};

CallToolWidget::Private::Private(::CallToolWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(q, &::CallToolWidget::toolChanged, q, [this](const QMcpTool &tool) {
        auto paramsLayout = qobject_cast<QFormLayout *>(params->layout());
        while (paramsLayout->rowCount() > 0)
            paramsLayout->removeRow(0);
        auto contentsLayout = qobject_cast<QFormLayout *>(contents->layout());
        while (contentsLayout->rowCount() > 0)
            contentsLayout->removeRow(0);

        name->setText(tool.name());
        description->setText(tool.description());
        const auto inputSchema = tool.inputSchema();
        const auto properties = inputSchema.properties();
        const auto required = inputSchema.required();
        const auto keys = properties.keys();
        for (const auto &key : keys) {
            const auto details = properties.value(key).toObject();
            const auto type = details.value("type"_L1).toString();
            auto title = key;
            if (details.contains("title"_L1))
                title = details.value("title"_L1).toString();
            if (required.contains(key))
                title += "*"_L1;
            if (type == "string"_L1) {
                auto lineEdit = new QLineEdit;
                lineEdit->setObjectName(key);
                paramsLayout->addRow(title, lineEdit);
            } else if (type == "number"_L1) {
                auto spinBox = new QDoubleSpinBox;
                spinBox->setMinimum(-1000);
                spinBox->setMaximum(1000);
                spinBox->setObjectName(key);
                paramsLayout->addRow(title, spinBox);
            } else {
                qWarning() << "type" << type << "not supported";
            }
            if (details.contains("description"_L1)) {
                paramsLayout->addRow("", new QLabel(details.value("description").toString()));
            }
        }
    });

    connect(call, &QPushButton::clicked, q, [this]() {
        auto contentsLayout = qobject_cast<QFormLayout *>(contents->layout());
        while (contentsLayout->rowCount() > 0)
            contentsLayout->removeRow(0);

        QMcpCallToolRequest request;
        auto params = request.params();
        params.setName(tool.name());
        auto arguments = params.arguments();
        description->setText(tool.description());
        const auto inputSchema = tool.inputSchema();
        const auto fields = inputSchema.properties();
        const auto required = inputSchema.required();
        const auto keys = fields.keys();
        for (const auto &key : keys) {
            const auto details = fields.value(key).toObject();
            const auto type = details.value("type"_L1).toString();
            if (type == "string"_L1) {
                const auto lineEdit = this->params->findChild<QLineEdit *>(key);
                const auto value = lineEdit->text();
                if (required.contains(key)) {
                    if (value.isEmpty()) {
                        qWarning() << key << "is required";
                        return;
                    } else {
                        arguments.insert(key, value);
                    }
                } else if (!value.isEmpty()) {
                    arguments.insert(key, value);
                }
            } else if (type == "number"_L1) {
                const auto spinBox = this->params->findChild<QDoubleSpinBox *>(key);
                const auto value = spinBox->value();
                if (required.contains(key)) {
                    arguments.insert(key, value);
                } else if (!qFuzzyIsNull(value)) {
                    arguments.insert(key, value);
                }
            } else {
                qWarning() << "type" << type << "not supported";
            }
        }
        params.setArguments(arguments);
        request.setParams(params);

        call->setEnabled(false);
        q->setLoading(true);
        q->client()->request(request, [this](const QMcpCallToolResult &result, const QMcpJSONRPCErrorError *error) {
            if (error) {
                emit q->errorOccurred(error->message());
                q->setLoading(false);
                call->setEnabled(true);
                return;
            }
            const auto contents = result.content();
            auto contentsLayout = qobject_cast<QFormLayout *>(this->contents->layout());
            for (const auto &content : contents) {
                if (content.refType() == "textContent"_L1) {
                    const auto textContent = content.textContent();
                    const auto text = textContent.text();
                    if (text.contains("\n"_L1)) {
                        auto textEdit = new QPlainTextEdit(textContent.text());
                        textEdit->setReadOnly(true);
                        contentsLayout->addRow(textContent.type() + ":", textEdit);
                    } else {
                        auto label = new QLabel(textContent.text());
                        contentsLayout->addRow(textContent.type() + ":", label);
                    }
                } else if (content.refType() == "imageContent"_L1) {
                    const auto imageContent = content.imageContent();
                    const auto mimeType = mimeDatabase.mimeTypeForName(imageContent.mimeType());
                    const auto suffix = mimeType.preferredSuffix().toUtf8();
                    auto data = QByteArray::fromBase64(imageContent.data());
                    QBuffer buffer(&data);
                    buffer.open(QBuffer::ReadOnly);
                    QImage image;
                    if (image.load(&buffer, suffix.constData())) {
                        auto label = new QLabel;
                        label->setScaledContents(true);
                        label->setPixmap(QPixmap::fromImage(image));
                        contentsLayout->addRow(imageContent.mimeType() + ":", label);
                    }
                } else if (content.refType() == "embeddedResource"_L1) {
                    const auto embeddedResource = content.embeddedResource();
                } else {
                    qWarning() << content.refType() << "not supported";
                }
            }
            q->setLoading(false);
            call->setEnabled(true);
        });
    });
}

CallToolWidget::CallToolWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

CallToolWidget::~CallToolWidget() = default;

QMcpTool CallToolWidget::tool() const
{
    return d->tool;
}

void CallToolWidget::setTool(const QMcpTool &tool)
{
    if (d->tool == tool) return;
    d->tool = tool;
    emit toolChanged(d->tool);
}
