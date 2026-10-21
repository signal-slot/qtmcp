# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The server the test runs as a child process, talking to it over pipes.
qt_internal_add_test_helper(stdioserver
    OVERRIDE_OUTPUT_DIRECTORY
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    SOURCES
        stdioserver.cpp
    LIBRARIES
        Qt::McpServer
)
# Loaded at runtime, so nothing else makes sure it is built
add_dependencies(stdioserver QMcpServerStdioPlugin)

qt_internal_add_test(tst_stdio
    SOURCES
        tst_stdio.cpp
    LIBRARIES
        Qt::Test
)
add_dependencies(tst_stdio stdioserver)
