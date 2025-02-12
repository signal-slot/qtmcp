# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_standard_project_setup()

qt_add_executable(mcpinspector
    main.cpp
    mainwindow.h mainwindow.cpp mainwindow.ui
    connectwidget.h connectwidget.cpp connectwidget.ui
    abstractwidget.h abstractwidget.cpp
    initializewidget.h initializewidget.cpp initializewidget.ui
    listtoolswidget.h listtoolswidget.cpp listtoolswidget.ui
    listresourceswidget.h listresourceswidget.cpp listresourceswidget.ui
    listpromptswidget.h listpromptswidget.cpp listpromptswidget.ui
    readresourcewidget.h readresourcewidget.cpp readresourcewidget.ui
    getpromptwidget.h getpromptwidget.cpp getpromptwidget.ui
    calltoolwidget.h calltoolwidget.cpp calltoolwidget.ui
    samplingwidget.h samplingwidget.cpp samplingwidget.ui
    listresourcetemplateswidget.h listresourcetemplateswidget.cpp listresourcetemplateswidget.ui
    resourcetemplatewidget.h resourcetemplatewidget.cpp resourcetemplatewidget.ui
    pingwidget.h pingwidget.cpp pingwidget.ui
    rootswidget.h rootswidget.cpp rootswidget.ui
)

target_link_libraries(mcpinspector PRIVATE
    Qt::Widgets
    Qt::McpClient
)
