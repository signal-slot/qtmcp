# Copyright (C) 2025 Signal Slot Inc.
# SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

add_subdirectory(qmcpabstracthttpserver)
add_subdirectory(qmcpserver)
add_subdirectory(qmcpserversession)
add_subdirectory(streamablehttp)

# These drive a real server over a loopback transport, so they need the sse
# backend plugins on both sides, just like tests/auto/mcpclient does.
if (NOT WIN32)
    add_subdirectory(mrtr)
    add_subdirectory(tasks_extension)
endif()
