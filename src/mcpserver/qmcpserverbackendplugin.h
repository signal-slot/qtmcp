// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERBACKENDPLUGIN_H
#define QMCPSERVERBACKENDPLUGIN_H

#include <QtMcpServer/qmcpserverglobal.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

#define QMcpServerBackendPluginFactoryInterface_iid "org.qt-project.Qt.QMcpServerBackendFactoryInterface"

class QMcpServerBackendInterface;

class Q_MCPSERVER_EXPORT QMcpServerBackendPlugin : public QObject
{
    Q_OBJECT
public:
    explicit QMcpServerBackendPlugin(QObject *parent = nullptr)
        : QObject(parent) {}

    virtual QMcpServerBackendInterface *create(const QString &key, QObject *parent = nullptr) = 0;
};

QT_END_NAMESPACE

#endif // QMCPSERVERBACKENDPLUGIN_H
