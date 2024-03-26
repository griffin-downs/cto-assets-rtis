# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


include("${CMAKE_SOURCE_DIR}/cmake/GenerateHeaders.cmake")

set(TARGET_NAME ${PROJECT_NAME})

add_executable(${TARGET_NAME} main.cpp)

generate_headers(
    TARGET_NAME ${TARGET_NAME}
    STRINGIFY_TOOL_TARGET_NAME ${BATCH_STRINGIFY_FILES_PROJECT}
    STRINGIFY_TOOL_PATH "${BATCH_STRINGIFY_FILES_TOOL_PATH}"
)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

set(COMMON_INCLUDES
    glfw
    glm
    "${CMAKE_BINARY_DIR}/include"
    "${CMAKE_SOURCE_DIR}/common/include"
    "${CMAKE_SOURCE_DIR}/include"
    ${COMMON_INCLUDE_DIRECTORY}
)
set(COMMON_LIBRARIES glfw glm::glm)

if(EMSCRIPTEN)
    set(TARGET_LINK_FLAGS
        -s USE_GLFW=3
        -s MAX_WEBGL_VERSION=2
        -flto
        -s FILESYSTEM=0
        # -s ALLOW_MEMORY_GROWTH=1
        # -s TOTAL_STACK=32MB
        # -s INITIAL_MEMORY=48MB
        --closure 1
        -O3
    )

    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        list(APPEND TARGET_LINK_FLAGS
            -s GL_ASSERTIONS=1
            -s GL_DEBUG=1
        )
    endif()

    string(REPLACE ";" " " SPACE_DELIMITED_LINK_FLAGS "${TARGET_LINK_FLAGS}")

    set_target_properties(${TARGET_NAME}
        PROPERTIES
            SUFFIX ".js"
            LINK_FLAGS "${SPACE_DELIMITED_LINK_FLAGS}"
    )

    target_compile_options(${TARGET_NAME}
        PRIVATE
            -O3
            -fconstexpr-steps=4290000000
            -fconstexpr-depth=4290000000
            -ferror-limit=0
    )

    target_include_directories(${TARGET_NAME}
        PRIVATE
            ${COMMON_INCLUDES}
    )
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            ${COMMON_LIBRARIES}
    )

    configure_file("${CMAKE_SOURCE_DIR}/cmake/index.html.in"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/index.html"
        @ONLY
    )

    configure_file("${CMAKE_SOURCE_DIR}/assets/qr-code.svg"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/qr-code.svg")
else()
    target_compile_options(${TARGET_NAME}
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -fconstexpr-steps=4290000000
            -fconstexpr-depth=4290000000
    )

    if(CMAKE_BUILD_TYPE STREQUAL Release)
        target_compile_options(${TARGET_NAME} PRIVATE -O3)
    endif()

    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)

    target_include_directories(${TARGET_NAME}
        PRIVATE
            OpenGL::GL
            GLEW::GLEW
            ${COMMON_INCLUDES}
    )
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            OpenGL::GL
            GLEW::GLEW
            ${COMMON_LIBRARIES}
    )
endif()
