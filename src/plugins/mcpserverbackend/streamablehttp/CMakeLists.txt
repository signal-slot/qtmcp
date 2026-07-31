# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

qt_internal_add_plugin(QMcpServerStreamableHttpPlugin
    OUTPUT_NAME qmcpserverstreamablehttp
    PLUGIN_TYPE mcpserverbackend
    SOURCES
        qmcpserverstreamablehttp.h qmcpserverstreamablehttp.cpp
        httpserver.h httpserver.cpp
    LIBRARIES
        Qt::McpCommon
        Qt::McpServer
        Qt::Network
)
