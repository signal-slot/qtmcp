# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

qt_internal_add_test(tst_qmcpcancellednotification
    SOURCES
        tst_qmcpcancellednotification.cpp
    LIBRARIES
        Qt::Test
        Qt::McpCommon
)