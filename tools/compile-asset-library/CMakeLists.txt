# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


set(TOOL_NAME CompileAssetLibrary)
add_executable(${TOOL_NAME} CompileAssetLibrary.cpp)

target_compile_options(${TOOL_NAME}
    PRIVATE
        ${COMMON_COMPILE_OPTIONS}
)

target_include_directories(${TOOL_NAME}
    PRIVATE
        "${COMMON_INCLUDE_DIRECTORY}"
        "${COMMON_TOOLS_INCLUDE_DIRECTORY}"
)