// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERBACKENDPLUGIN_H
#define QMCPSERVERBACKENDPLUGIN_H

#include <QtMcpServer/qmcpserverglobal.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

#define QMcpServerBackendPluginFactoryInterface_iid "org.qt-project.Qt.QMcpServerBackendFactoryInterface"

class QMcpServerBackendInterface;

/*!
    \class QMcpServerBackendPlugin
    \inmodule QtMcpServer
    \brief The QMcpServerBackendPlugin class is the base class for MCP server backend plugins.

    This class defines the interface for plugins that provide different backend implementations
    for MCP servers. Each plugin can create backend instances based on a key identifier.

    To create a new backend plugin:
    \list
    \li Inherit from QMcpServerBackendPlugin
    \li Implement the create() method to return your backend implementation
    \li Export your plugin using Q_PLUGIN_METADATA
    \endlist

    Example:
    \code
    class MyServerBackendPlugin : public QMcpServerBackendPlugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID QMcpServerBackendPluginFactoryInterface_iid)
    public:
        QMcpServerBackendInterface *create(const QString &key, QObject *parent = nullptr) override
        {
            if (key == "mybackend")
                return new MyServerBackendImplementation(parent);
            return nullptr;
        }
    };
    \endcode
*/
class Q_MCPSERVER_EXPORT QMcpServerBackendPlugin : public QObject
{
    Q_OBJECT
public:
    /*!
        Constructs a server backend plugin with the given parent.
        \param parent The parent object
    */
    explicit QMcpServerBackendPlugin(QObject *parent = nullptr)
        : QObject(parent) {}

    /*!
        Creates a new backend instance for the given key.
        Must be implemented by plugin classes.

        \param key The identifier for the type of backend to create
        \param parent The parent object for the new backend instance
        \return A new backend instance, or nullptr if the key is not supported
    */
    virtual QMcpServerBackendInterface *create(const QString &key, QObject *parent = nullptr) = 0;
};

QT_END_NAMESPACE

#endif // QMCPSERVERBACKENDPLUGIN_H
