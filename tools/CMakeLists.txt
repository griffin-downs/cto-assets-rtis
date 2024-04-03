# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


cmake_minimum_required(VERSION 3.28)

project(Tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(COMMON_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${TOOLS_BINARY_DIRECTORY}")

add_subdirectory(generate-asset-data)
add_subdirectory(batch-stringify-files)
add_subdirectory(compile-asset-library)
