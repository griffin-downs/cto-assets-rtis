# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


cmake_minimum_required(VERSION 3.28)

project(cto-assets-rtis LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(EMSCRIPTEN)
    add_definitions("-std=c++23")
else()
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

add_subdirectory(src)
