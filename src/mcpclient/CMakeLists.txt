# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_internal_add_module(McpClient
    PLUGIN_TYPES mcpclientbackend
    SOURCES
        qmcpclientglobal.h
        qmcpclient.h qmcpclient.cpp
        qmcpclientbackendinterface.h qmcpclientbackendinterface.cpp
        qmcpclientbackendplugin.h

    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_LIBRARIES
        Qt::McpCommon
        Qt::CorePrivate
)
