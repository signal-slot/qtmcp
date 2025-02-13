# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

find_package(Qt6 REQUIRED COMPONENTS Core Test)

qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_internal_add_test(tst_qmcpserver
    SOURCES
        tst_qmcpserver.cpp
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../mcpcommon
        ${CMAKE_SOURCE_DIR}/src
    LIBRARIES
        Qt::McpServer
        Qt::McpCommon
        Qt::Test
        Qt::TestPrivate
        Qt::Core
        Qt::CorePrivate
)

target_link_libraries(tst_qmcpserver PRIVATE
    Qt::McpServer
    Qt::McpCommon
    Qt::Test
    Qt::Core
)

target_compile_definitions(tst_qmcpserver PRIVATE
    QT_USE_QSTRINGBUILDER
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x050F00
)
