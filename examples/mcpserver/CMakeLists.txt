# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(echo)
if(TARGET Qt6::Gui)
    add_subdirectory(window)
endif()
