// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTBACKENDPLUGIN_H
#define QMCPCLIENTBACKENDPLUGIN_H

#include <QtMcpClient/qmcpclientglobal.h>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

#define QMcpClientBackendPluginFactoryInterface_iid "org.qt-project.Qt.QMcpClientBackendFactoryInterface"

class QMcpClientBackendInterface;

/*!
    \class QMcpClientBackendPlugin
    \inmodule QtMcpClient
    \brief The QMcpClientBackendPlugin class is the base class for MCP client backend plugins.

    This class defines the interface for plugins that provide different backend implementations
    for MCP clients. Each plugin can create backend instances based on a key identifier.

    To create a new backend plugin:
    \list
    \li Inherit from QMcpClientBackendPlugin
    \li Implement the create() method to return your backend implementation
    \li Export your plugin using Q_PLUGIN_METADATA
    \endlist

    Example:
    \code
    class MyBackendPlugin : public QMcpClientBackendPlugin
    {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID QMcpClientBackendPluginFactoryInterface_iid)
    public:
        QMcpClientBackendInterface *create(const QString &key, QObject *parent = nullptr) override
        {
            if (key == "mybackend")
                return new MyBackendImplementation(parent);
            return nullptr;
        }
    };
    \endcode
*/
class Q_MCPCLIENT_EXPORT QMcpClientBackendPlugin : public QObject
{
    Q_OBJECT
public:
    /*!
        Constructs a client backend plugin with the given parent.
        \param parent The parent object
    */
    explicit QMcpClientBackendPlugin(QObject *parent = nullptr)
        : QObject(parent) {}

    /*!
        Creates a new backend instance for the given key.
        Must be implemented by plugin classes.

        \param key The identifier for the type of backend to create
        \param parent The parent object for the new backend instance
        \return A new backend instance, or nullptr if the key is not supported
    */
    virtual QMcpClientBackendInterface *create(const QString &key, QObject *parent = nullptr) = 0;
};

QT_END_NAMESPACE

#endif // QMCPCLIENTBACKENDPLUGIN_H
