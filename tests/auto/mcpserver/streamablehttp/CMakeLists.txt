# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

# QMcpServer's templated request handlers use C++20 constrained templates.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_internal_add_test(tst_streamablehttp
    SOURCES
        tst_streamablehttp.cpp
    LIBRARIES
        Qt::McpCommon
        Qt::McpServer
        Qt::Network
        Qt::Test
)
