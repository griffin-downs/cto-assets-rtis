# ==============================================================================
# Copyright (C) 2025, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


add_executable(${PROJECT_NAME} main.cpp)

include("${CMAKE_SOURCE_DIR}/cmake/CompileAssets.cmake")
set(COMPILED_ASSETS_INCLUDE_DIRECTORY "${CMAKE_BINARY_DIR}/compiled-assets")
compile_assets(
    TARGET_NAME ${PROJECT_NAME}
    INPUT_FILES
        "${CMAKE_SOURCE_DIR}/assets/OuterWireDodecahedron.obj"
        "${CMAKE_SOURCE_DIR}/assets/VoronoiSphere.obj"
    OUTPUT_DIRECTORY
        "${COMPILED_ASSETS_INCLUDE_DIRECTORY}"
)

include("${CMAKE_SOURCE_DIR}/cmake/BatchStringifyFiles.cmake")
set(STRINGIFIED_SHADERS_INCLUDE_DIRECTORY
    "${CMAKE_BINARY_DIR}/stringified-shaders"
)
batch_stringify_files(
    TARGET_NAME ${PROJECT_NAME}
    INPUT_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/BaseVertexShader.glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/BaseFragmentShader.glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/SsaaVertexShader.glsl"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/SsaaFragmentShader.glsl"
    OUTPUT_DIRECTORY
        "${STRINGIFIED_SHADERS_INCLUDE_DIRECTORY}"
)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

set(COMMON_INCLUDES
    glfw
    glm
    "${COMPILED_ASSETS_INCLUDE_DIRECTORY}"
    "${STRINGIFIED_SHADERS_INCLUDE_DIRECTORY}"
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
            -s ASSERTIONS=1
            -s GL_DEBUG=1
        )
    endif()

    string(REPLACE ";" " " SPACE_DELIMITED_LINK_FLAGS "${TARGET_LINK_FLAGS}")

    set_target_properties(${PROJECT_NAME}
        PROPERTIES
            SUFFIX ".js"
            LINK_FLAGS "${SPACE_DELIMITED_LINK_FLAGS}"
    )

    target_compile_options(${PROJECT_NAME}
        PRIVATE
            -O3
            -fconstexpr-steps=4290000000
            -fconstexpr-depth=4290000000
            -ferror-limit=0
    )

    target_include_directories(${PROJECT_NAME}
        PRIVATE
            ${COMMON_INCLUDES}
    )
    target_link_libraries(${PROJECT_NAME}
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
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -fconstexpr-steps=4290000000
            -fconstexpr-depth=4290000000
    )

    if(CMAKE_BUILD_TYPE STREQUAL Release)
        target_compile_options(${PROJECT_NAME} PRIVATE -O3)
    endif()

    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)

    target_include_directories(${PROJECT_NAME}
        PRIVATE
            OpenGL::GL
            GLEW::GLEW
            ${COMMON_INCLUDES}
    )
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            OpenGL::GL
            GLEW::GLEW
            ${COMMON_LIBRARIES}
    )
endif()
