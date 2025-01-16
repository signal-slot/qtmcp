# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

qt_internal_add_plugin(QMcpClientSsePlugin
    OUTPUT_NAME qmcpclientsse
    PLUGIN_TYPE mcpclientbackend
    SOURCES
        qmcpclientsse.h qmcpclientsse.cpp
    LIBRARIES
        Qt::Network
        Qt::McpClient
)
