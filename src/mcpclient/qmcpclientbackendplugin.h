// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTBACKENDPLUGIN_H
#define QMCPCLIENTBACKENDPLUGIN_H

#include <QtMcpClient/qmcpclientglobal.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

#define QMcpClientBackendPluginFactoryInterface_iid "org.qt-project.Qt.QMcpClientBackendFactoryInterface"

class QMcpClientBackendInterface;

class Q_MCPCLIENT_EXPORT QMcpClientBackendPlugin : public QObject
{
    Q_OBJECT
public:
    explicit QMcpClientBackendPlugin(QObject *parent = nullptr)
        : QObject(parent) {}

    virtual QMcpClientBackendInterface *create(const QString &key, QObject *parent = nullptr) = 0;
};

QT_END_NAMESPACE

#endif // QMCPCLIENTBACKENDPLUGIN_H
