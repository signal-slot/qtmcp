# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_standard_project_setup()

qt_add_executable(echo
    main.cpp
    mcpserver.h mcpserver.cpp
)

target_link_libraries(echo PRIVATE
    Qt::McpServer
)
