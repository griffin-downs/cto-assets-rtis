# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


set(TOOL_NAME GenerateAssetData)
add_executable(${TOOL_NAME} GenerateAssetData.cpp)

find_package(assimp REQUIRED)

set(ASSIMP_ISSUE_2606_MITIGATION -Wno-microsoft-enum-value)
set(ASSIMP_DEPRECATED_IMPLICIT_COPY_CONSTRUCTOR_MITIGATION -Wno-deprecated-copy)

target_compile_options(${TOOL_NAME}
    PRIVATE
        ${COMMON_COMPILE_OPTIONS}
        ${ASSIMP_ISSUE_2606_MITIGATION}
        ${ASSIMP_DEPRECATED_IMPLICIT_COPY_CONSTRUCTOR_MITIGATION}
)

target_include_directories(${TOOL_NAME}
    PRIVATE
        assimp::assimp
        "${COMMON_INCLUDE_DIRECTORY}"
        "${COMMON_TOOLS_INCLUDE_DIRECTORY}"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${TOOL_NAME}
    PRIVATE
        assimp::assimp
)