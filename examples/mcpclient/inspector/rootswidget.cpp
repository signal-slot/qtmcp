// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "rootswidget.h"
#include "ui_rootswidget.h"

#include <QtMcpCommon/QMcpListRootsRequest>
#include <QtMcpCommon/QMcpListRootsResult>
#include <QtMcpCommon/QMcpRootsListChangedNotification>

class RootsWidget::Private : public Ui::RootsWidget
{
public:
    Private(::RootsWidget *parent);

private:
    ::RootsWidget *q;
};

RootsWidget::Private::Private(::RootsWidget *parent)
    : q(parent)
{
    setupUi(q);

    connect(q, &AbstractWidget::clientChanged, q, [this](QMcpClient *client) {
        if (client) {
            client->addRequestHandler(
                [this](const QMcpListRootsRequest &request, QMcpJSONRPCErrorError *) -> QMcpListRootsResult {
                    Q_UNUSED(request);
                    QMcpListRootsResult ret;
                    QList<QMcpRoot> roots;
                    const QStringList lines = this->roots->toPlainText().split("\n"_L1);
                    for (const auto &line : lines) {
                        QMcpRoot root;
                        int space = line.lastIndexOf(' '_L1);
                        if (space < 0) {
                            root.setUri(QUrl(line));
                        } else {
                            root.setName(line.left(space));
                            root.setUri(QUrl(line.mid(space + 1)));
                        }
                        roots.append(root);
                    }
                    ret.setRoots(roots);
                    return ret;
                });
        }
    });

    connect(notify, &QPushButton::clicked, q, [this]() {
        q->setLoading(true);
        QMcpRootsListChangedNotification notification;
        q->client()->notify(notification);
        q->setLoading(false);
    });
}

RootsWidget::RootsWidget(QWidget *parent)
    : AbstractWidget(parent)
    , d(new Private(this))
{}

RootsWidget::~RootsWidget() = default;

