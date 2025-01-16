# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_internal_add_test(tst_qmcpclient
    SOURCES
        tst_qmcpclient.cpp
    LIBRARIES
        Qt::Test
        Qt::Core
        Qt::McpClient
        Qt::McpCommon
)
