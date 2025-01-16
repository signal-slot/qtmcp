# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_internal_add_test(tst_qmcpserver
    SOURCES
        tst_qmcpserver.cpp
    PUBLIC_LIBRARIES
        Qt::Test
        Qt::McpServer
)
