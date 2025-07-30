# ==============================================================================
# Copyright (C) 2025, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


set(TOOL_NAME BatchStringifyFiles)
add_executable(${TOOL_NAME} BatchStringifyFiles.cpp)

target_compile_options(${TOOL_NAME}
    PRIVATE
        ${COMMON_COMPILE_OPTIONS}
)

target_include_directories(${TOOL_NAME}
    PRIVATE
        "${COMMON_INCLUDE_DIRECTORY}"
        "${COMMON_TOOLS_INCLUDE_DIRECTORY}"
)
