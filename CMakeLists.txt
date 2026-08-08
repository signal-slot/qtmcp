# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

cmake_minimum_required(VERSION 3.16.0)

include(.cmake.conf)
project(QtMcp
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt Mcp Libraries"
    HOMEPAGE_URL "https://github.com/signal-slot/qtmcp"
    LANGUAGES CXX C
)

find_package(Qt6 6.8 CONFIG REQUIRED COMPONENTS
    BuildInternals
    Core
)
find_package(Qt6 6.8 CONFIG OPTIONAL_COMPONENTS
    Gui
    Widgets
    Network
)

qt_internal_project_setup()

qt_build_repo()

# Qt MCP is an independent library that happens to use Qt's module build
# system; offer the modules under the library's own namespace as well
# (QtMcp::Common/Client/Server) so consumers do not have to reach into Qt6::.
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtMcpConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/QtMcpConfig.cmake
    INSTALL_DESTINATION ${INSTALL_LIBDIR}/cmake/QtMcp
)
# The QtMcp package carries the MCP-revision-based release version, not the
# Qt-shaped module version. API only grows across revisions, hence
# AnyNewerVersion.
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/QtMcpConfigVersion.cmake
    VERSION ${QTMCP_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/QtMcpConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/QtMcpConfigVersion.cmake
    DESTINATION ${INSTALL_LIBDIR}/cmake/QtMcp
)
