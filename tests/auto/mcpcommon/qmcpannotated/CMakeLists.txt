# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

set(CMAKE_CXX_STANDARD 20)

qt_internal_add_test(tst_qmcpannotated
    SOURCES
        tst_qmcpannotated.cpp
    LIBRARIES
        Qt::Test
        Qt::McpCommon
)
