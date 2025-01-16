# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

qt_internal_add_plugin(QMcpClientStdioPlugin
    OUTPUT_NAME qmcpclientstdio
    PLUGIN_TYPE mcpclientbackend
    SOURCES
        qmcpclientstdio.h qmcpclientstdio.cpp
    LIBRARIES
        Qt::McpClient
)
