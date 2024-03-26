# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


cmake_minimum_required(VERSION 3.28)

include(ExternalProject)

set(COMMON_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/common/include")

set(BATCH_STRINGIFY_FILES_PROJECT BatchStringifyFiles)
set(BATCH_STRINGIFY_FILES_BINARY_DIR
    "${CMAKE_BINARY_DIR}/tools/batch-stringify-files"
)
set(BATCH_STRINGIFY_FILES_TOOL_PATH
    "${BATCH_STRINGIFY_FILES_BINARY_DIR}/${BATCH_STRINGIFY_FILES_PROJECT}"
)
ExternalProject_Add(
    ${BATCH_STRINGIFY_FILES_PROJECT}
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/tools/batch-stringify-files"
    BINARY_DIR "${BATCH_STRINGIFY_FILES_BINARY_DIR}"
    INSTALL_COMMAND ""
    CMAKE_ARGS
        -DEXECUTABLE_NAME=${BATCH_STRINGIFY_FILES_PROJECT}
        -DCOMMON_INCLUDE_DIRECTORY=${CMAKE_SOURCE_DIR}/common/include
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
)

project(cto-assets-rtis LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(EMSCRIPTEN)
    add_definitions("-std=c++23")
else()
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_subdirectory(src)
