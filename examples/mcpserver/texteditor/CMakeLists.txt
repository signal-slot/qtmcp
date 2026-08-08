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

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

install(TARGETS texteditor
    RUNTIME DESTINATION "${INSTALL_EXAMPLESDIR}/mcpserver/texteditor"
    BUNDLE DESTINATION "${INSTALL_EXAMPLESDIR}/mcpserver/texteditor"
    LIBRARY DESTINATION "${INSTALL_EXAMPLESDIR}/mcpserver/texteditor"
)
