# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_add_executable(echo
    main.cpp
)

target_link_libraries(echo PRIVATE
    Qt::McpServer
)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

install(TARGETS echo
    RUNTIME DESTINATION "${INSTALL_EXAMPLESDIR}/mcpserver/echo"
    BUNDLE DESTINATION "${INSTALL_EXAMPLESDIR}/mcpserver/echo"
    LIBRARY DESTINATION "${INSTALL_EXAMPLESDIR}/mcpserver/echo"
)
