# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


cmake_minimum_required(VERSION 3.28)

project(BatchStringifyFiles LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(EXECUTABLE_NAME BatchStringifyFiles)
add_executable(${EXECUTABLE_NAME} BatchStringifyFiles.cpp)

target_compile_options(${EXECUTABLE_NAME}
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror
)

target_include_directories(${EXECUTABLE_NAME}
    PRIVATE
        "${COMMON_INCLUDE_DIRECTORY}"
)
