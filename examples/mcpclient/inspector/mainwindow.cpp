// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectwidget.h"
#include "initializewidget.h"
#include "pingwidget.h"
#include "listresourceswidget.h"
#include "readresourcewidget.h"
#include "listresourcetemplateswidget.h"
#include "resourcetemplatewidget.h"
#include "listtoolswidget.h"
#include "calltoolwidget.h"
#include "listpromptswidget.h"
#include "getpromptwidget.h"
#include "samplingwidget.h"

#include <QtCore/QSettings>
#include <QtWidgets/QSplitter>
#include <QtMcpClient/QMcpClient>

class MainWindow::Private : public Ui::MainWindow
{
public:
    Private(::MainWindow *parent);
    ~Private();

    enum Type {
        Connect = QTreeWidgetItem::UserType,
        Initialize,
        Ping,
        ListResources,
        ReadResource,
        ListResourceTemplates,
        ResourceTemplate,
        ListPrompts,
        GetPrompt,
        ListTools,
        CallTool,
        Sampling,
        // Roots,
    };

private:
    ::MainWindow *q;
public:
    QSplitter *splitter;
    QSettings settings;
    QMcpClient *client = nullptr;
};

MainWindow::Private::Private(::MainWindow *parent)
    : q(parent)
    , splitter(new QSplitter)
{
    settings.beginGroup(q->metaObject()->className());
    setupUi(q);

    q->statusBar()->addPermanentWidget(progressBar, 1);
    progressBar->hide();

    auto centralWidget = q->centralWidget();
    const auto children = centralWidget->findChildren<QWidget *>(Qt::FindDirectChildrenOnly);
    for (auto *child : children) {
        splitter->addWidget(child);
    }
    q->setCentralWidget(splitter);
    centralWidget->deleteLater();

    q->restoreState(settings.value("state").toByteArray());
    q->restoreGeometry(settings.value("geometry").toByteArray());
    splitter->restoreState(settings.value("splitter").toByteArray());

    while (stackedWidget->count() > 0)
        delete stackedWidget->currentWidget();

    auto connectWidget = new ConnectWidget;
    connect(connectWidget, &ConnectWidget::clientChanged, q, [this](QMcpClient *client) {
        this->client = client;
        if (client) {
            auto item = new QTreeWidgetItem({ "Initialize" }, Private::Initialize);
            treeWidget->addTopLevelItem(item);
            treeWidget->setCurrentItem(item);
        } else {
            treeWidget->clear();
            auto item = new QTreeWidgetItem({ "Connect" }, Private::Connect);
            treeWidget->addTopLevelItem(item);
        }
        for (int i = 0; i < stackedWidget->count(); i++)
            qobject_cast<AbstractWidget *>(stackedWidget->widget(i))->setClient(client);
    });
    stackedWidget->addWidget(connectWidget);

    auto initializeWidget = new InitializeWidget;
    connect(initializeWidget, &InitializeWidget::initialized, q, [this]() {
        treeWidget->addTopLevelItem(new QTreeWidgetItem({ "Ping" }, Private::Ping));
        treeWidget->addTopLevelItem(new QTreeWidgetItem({ "Resources" }, Private::ListResources));
        treeWidget->addTopLevelItem(new QTreeWidgetItem({ "Resource Templates" }, Private::ListResourceTemplates));
        treeWidget->addTopLevelItem(new QTreeWidgetItem({ "Prompts" }, Private::ListPrompts));
        treeWidget->addTopLevelItem(new QTreeWidgetItem({ "Tools" }, Private::ListTools));
        treeWidget->addTopLevelItem(new QTreeWidgetItem({ "Sampling" }, Private::Sampling));
    });
    stackedWidget->addWidget(initializeWidget);

    stackedWidget->addWidget(new PingWidget);

    auto resources = new ListResourcesWidget;
    connect(resources, &ListResourcesWidget::resourcesChanged, q, [this](const QList<QMcpResource> &resources) {
        // find type = ListResource from toplevel items
        for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
            auto topLevelItem = treeWidget->topLevelItem(i);
            if (topLevelItem->type() != Private::ListResources)
                continue;

            // merge child items and resources
            auto resourcesToBeAdded = resources;
            for (int i = topLevelItem->childCount() - 1; i >= 0; i--) {
                auto child = topLevelItem->child(i);
                auto resource = child->data(0, Qt::UserRole).value<QMcpResource>();
                if (resourcesToBeAdded.contains(resource)) {
                    resourcesToBeAdded.removeOne(resource);
                } else {
                    topLevelItem->removeChild(child);
                }
            }

            for (const auto &resource : resourcesToBeAdded) {
                auto item = new QTreeWidgetItem({ resource.name() }, Private::ReadResource);
                item->setData(0, Qt::UserRole, QVariant::fromValue(resource));
                topLevelItem->addChild(item);
            }

            treeWidget->expandItem(topLevelItem);
            break;
        }
    });
    stackedWidget->addWidget(resources);
    stackedWidget->addWidget(new ReadResourceWidget);

    auto resourceTemplates = new ListResourceTemplatesWidget;
    connect(resourceTemplates, &ListResourceTemplatesWidget::resourceTemplatesChanged, q, [this](const QList<QMcpResourceTemplate> &resourceTemplates) {
        // find type = ListResource from toplevel items
        for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
            auto topLevelItem = treeWidget->topLevelItem(i);
            if (topLevelItem->type() != Private::ListResourceTemplates)
                continue;

            // merge child items and resources
            auto resourceTemplatesToBeAdded = resourceTemplates;
            for (int i = topLevelItem->childCount() - 1; i >= 0; i--) {
                auto child = topLevelItem->child(i);
                auto resourceTemplate = child->data(0, Qt::UserRole).value<QMcpResourceTemplate>();
                if (resourceTemplatesToBeAdded.contains(resourceTemplate)) {
                    resourceTemplatesToBeAdded.removeOne(resourceTemplate);
                } else {
                    topLevelItem->removeChild(child);
                }
            }

            for (const auto &resourceTemplate : resourceTemplatesToBeAdded) {
                auto item = new QTreeWidgetItem({ resourceTemplate.name() }, Private::ResourceTemplate);
                item->setData(0, Qt::UserRole, QVariant::fromValue(resourceTemplate));
                topLevelItem->addChild(item);
            }

            treeWidget->expandItem(topLevelItem);
            break;
        }
    });
    stackedWidget->addWidget(resourceTemplates);
    stackedWidget->addWidget(new ResourceTemplateWidget);

    auto prompts = new ListPromptsWidget;
    connect(prompts, &ListPromptsWidget::promptsChanged, q, [this](const QList<QMcpPrompt> &prompts) {
        // find type = ListPrompt from toplevel items
        for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
            auto topLevelItem = treeWidget->topLevelItem(i);
            if (topLevelItem->type() != Private::ListPrompts)
                continue;

            // merge child items and prompts
            auto promptsToBeAdded = prompts;
            for (int i = topLevelItem->childCount() - 1; i >= 0; i--) {
                auto child = topLevelItem->child(i);
                auto prompt = child->data(0, Qt::UserRole).value<QMcpPrompt>();
                if (promptsToBeAdded.contains(prompt)) {
                    promptsToBeAdded.removeOne(prompt);
                } else {
                    topLevelItem->removeChild(child);
                }
            }

            for (const auto &prompt : promptsToBeAdded) {
                auto item = new QTreeWidgetItem({ prompt.name() }, Private::GetPrompt);
                item->setData(0, Qt::UserRole, QVariant::fromValue(prompt));
                topLevelItem->addChild(item);
            }

            treeWidget->expandItem(topLevelItem);
            break;
        }
    });
    stackedWidget->addWidget(prompts);
    stackedWidget->addWidget(new GetPromptWidget);

    auto tools = new ListToolsWidget;
    connect(tools, &ListToolsWidget::toolsChanged, q, [this](const QList<QMcpTool> &tools) {
        // find type = ListTool from toplevel items
        for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
            auto topLevelItem = treeWidget->topLevelItem(i);
            if (topLevelItem->type() != Private::ListTools)
                continue;

            // merge child items and tools
            auto toolsToBeAdded = tools;
            for (int i = topLevelItem->childCount() - 1; i >= 0; i--) {
                auto child = topLevelItem->child(i);
                auto tool = child->data(0, Qt::UserRole).value<QMcpTool>();
                if (toolsToBeAdded.contains(tool)) {
                    toolsToBeAdded.removeOne(tool);
                } else {
                    topLevelItem->removeChild(child);
                }
            }

            for (const auto &tool : toolsToBeAdded) {
                auto item = new QTreeWidgetItem({ tool.name() }, Private::CallTool);
                item->setData(0, Qt::UserRole, QVariant::fromValue(tool));
                topLevelItem->addChild(item);
            }

            treeWidget->expandItem(topLevelItem);
            break;
        }
    });
    stackedWidget->addWidget(tools);
    stackedWidget->addWidget(new CallToolWidget);

    stackedWidget->addWidget(new SamplingWidget);

    connect(treeWidget, &QTreeWidget::currentItemChanged, q, [this](QTreeWidgetItem *current) {
        auto currentWidget = qobject_cast<AbstractWidget *>(stackedWidget->currentWidget());
        if (currentWidget) {
            disconnect(currentWidget, &AbstractWidget::loadingChanged, progressBar, &QProgressBar::setVisible);
            disconnect(currentWidget, &AbstractWidget::loadingChanged, q, &::MainWindow::setBusy);
            disconnect(currentWidget, &AbstractWidget::errorOccurred, q, &::MainWindow::showErrorMessage);
        }
        if (!current) {
            stackedWidget->setCurrentIndex(-1);
            return;
        }
        stackedWidget->setCurrentIndex(current->type() - QTreeWidgetItem::UserType);
        currentWidget = qobject_cast<AbstractWidget *>(stackedWidget->currentWidget());
        if (currentWidget) {
            connect(currentWidget, &AbstractWidget::loadingChanged, progressBar, &QProgressBar::setVisible);
            connect(currentWidget, &AbstractWidget::loadingChanged, q, &::MainWindow::setBusy);
            connect(currentWidget, &AbstractWidget::errorOccurred, q, &::MainWindow::showErrorMessage);
            progressBar->setValue(currentWidget->isLoading());
            q->setBusy(currentWidget->isLoading());
        } else {
            progressBar->setValue(false);
            q->setBusy(false);
        }

        switch (current->type()) {
        case Private::ReadResource: {
            const auto resource = current->data(0, Qt::UserRole).value<QMcpResource>();
            auto *widget = qobject_cast<ReadResourceWidget *>(currentWidget);
            widget->setResource(resource);
            break; }
        case Private::ResourceTemplate: {
            const auto resourceTemplate = current->data(0, Qt::UserRole).value<QMcpResourceTemplate>();
            auto *widget = qobject_cast<ResourceTemplateWidget *>(currentWidget);
            widget->setResourceTemplate(resourceTemplate);
            break; }
        case Private::GetPrompt: {
            const auto prompt = current->data(0, Qt::UserRole).value<QMcpPrompt>();
            auto *widget = qobject_cast<GetPromptWidget *>(currentWidget);
            widget->setPrompt(prompt);
            break; }
        case Private::CallTool: {
            const auto tool = current->data(0, Qt::UserRole).value<QMcpTool>();
            auto *widget = qobject_cast<CallToolWidget *>(currentWidget);
            widget->setTool(tool);
            break; }
        default:
            break;
        }
    });

    auto item = new QTreeWidgetItem({ "Connect" }, Private::Connect);
    treeWidget->addTopLevelItem(item);
}

MainWindow::Private::~Private()
{
    settings.setValue("splitter", splitter->saveState());
    settings.setValue("geometry", q->saveGeometry());
    settings.setValue("state", q->saveState());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(new Private(this))
{}

MainWindow::~MainWindow() = default;

void MainWindow::setBusy(bool busy)
{
    if (busy)
        QApplication::setOverrideCursor(Qt::WaitCursor);
    else
        QApplication::restoreOverrideCursor();
}

void MainWindow::showErrorMessage(const QString &message)
{
    d->statusbar->showMessage(message, 5000);
}
