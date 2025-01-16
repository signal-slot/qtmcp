# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

qt_internal_add_plugin(QMcpServerSsePlugin
    OUTPUT_NAME qmcpserversse
    PLUGIN_TYPE mcpserverbackend
    SOURCES
        qmcpserversse.h qmcpserversse.cpp
    LIBRARIES
	Qt::McpServer
	Qt::HttpServer
        Qt::Concurrent
)
