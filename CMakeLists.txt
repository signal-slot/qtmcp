# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

cmake_minimum_required(VERSION 3.16.0)

include(.cmake.conf)
project(QtMcp
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt Mcp Libraries"
    HOMEPAGE_URL "https://signal-slot.co.jp/"
    LANGUAGES CXX C
)

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS
    BuildInternals
    Core
)
find_package(Qt6 ${PROJECT_VERSION} CONFIG OPTIONAL_COMPONENTS
    Gui
    Widgets
    Network
    HttpServer
)

qt_internal_project_setup()

qt_build_repo()
