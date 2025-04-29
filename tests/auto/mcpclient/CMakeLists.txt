# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

if (NOT WIN32)
    add_subdirectory(qmcpclient)
    add_subdirectory(version_negotiation)
endif()
