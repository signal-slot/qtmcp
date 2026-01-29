# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_add_executable(texteditor
    main.cpp
    mainwindow.h mainwindow.cpp
)

target_link_libraries(texteditor PRIVATE
    Qt::McpServer
    Qt::Widgets
)
