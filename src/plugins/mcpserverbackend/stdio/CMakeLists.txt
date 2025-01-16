# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

qt_internal_add_plugin(QMcpServerStdioPlugin
    OUTPUT_NAME qmcpserverstdio
    PLUGIN_TYPE mcpserverbackend
    SOURCES
        qmcpserverstdio.h qmcpserverstdio.cpp
    LIBRARIES
        Qt::Core
	Qt::McpServer
        Qt::McpCommon
)
