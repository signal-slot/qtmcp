# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

qt_internal_add_test(tst_qmcpsetlevelrequestparams
    SOURCES
        tst_qmcpsetlevelrequestparams.cpp
    LIBRARIES
        Qt::McpCommon
        Qt::Test
)