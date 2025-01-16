# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#####################################################################
## McpServer Module:
#####################################################################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_internal_add_module(McpServer
    PLUGIN_TYPES mcpserverbackend
    SOURCES
        qmcpserverglobal.h
        qmcpserver.h qmcpserver.cpp
        qmcpserverbackendplugin.h
        qmcpserverbackendinterface.h qmcpserverbackendinterface.cpp
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_LIBRARIES
        Qt::McpCommon
        Qt::CorePrivate
    DEFINES
        QT_BUILD_MCPSERVER_LIB
)

## Scopes:
#####################################################################

qt_internal_extend_target(McpServer CONDITION TARGET Qt::Gui
    PUBLIC_LIBRARIES
        Qt::Gui
)
