# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


include("${CMAKE_SOURCE_DIR}/cmake/BatchStringifyFiles.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/MakeNamedTuple.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ParseAndValidateArguments.cmake")

set(GENERATED_HEADER_DIR "${CMAKE_BINARY_DIR}/include")

function(generate_headers)
    set(prefix _GENERATE_HEADERS)
    parse_and_validate_arguments(
        PREFIX ${prefix}
        SINGLE_VALUE_PARAMETERS
            TARGET_NAME
            STRINGIFY_TOOL_TARGET_NAME
            STRINGIFY_TOOL_PATH
        ARGN ${ARGN}
    )

    make_named_tuple(
        OUTPUT_VARIABLE vertex_shader_arguments
        PARAMETER_SET "${BATCH_STRINGIFY_FILES_CPP_PARAMETER_SET}"
        ARGUMENTS
            INPUT_FILE "${CMAKE_SOURCE_DIR}/src/shaders/VertexShader.glsl"
            OUTPUT_FILE "${GENERATED_HEADER_DIR}/VertexShader.h"
            TYPE_NAME "VertexShaderCode"
    )

    make_named_tuple(
        OUTPUT_VARIABLE fragment_shader_arguments
        PARAMETER_SET "${BATCH_STRINGIFY_FILES_CPP_PARAMETER_SET}"
        ARGUMENTS
            INPUT_FILE "${CMAKE_SOURCE_DIR}/src/shaders/FragmentShader.glsl"
            OUTPUT_FILE "${GENERATED_HEADER_DIR}/FragmentShader.h"
            TYPE_NAME "FragmentShaderCode"
    )

    make_named_tuple(
        OUTPUT_VARIABLE rubiks_cube_model_arguments
        PARAMETER_SET "${BATCH_STRINGIFY_FILES_CPP_PARAMETER_SET}"
        ARGUMENTS
            INPUT_FILE "${CMAKE_SOURCE_DIR}/assets/RubiksCube.obj"
            OUTPUT_FILE "${GENERATED_HEADER_DIR}/RubiksCubeObj.h"
            TYPE_NAME "RubiksCubeObj"
    )

    make_named_tuple(
        OUTPUT_VARIABLE rubiks_cube_material_library_arguments
        PARAMETER_SET "${BATCH_STRINGIFY_FILES_CPP_PARAMETER_SET}"
        ARGUMENTS
            INPUT_FILE "${CMAKE_SOURCE_DIR}/assets/RubiksCube.mtl"
            OUTPUT_FILE "${GENERATED_HEADER_DIR}/RubiksCubeMtl.h"
            TYPE_NAME "RubiksCubeMtl"
    )

    batch_stringify_files(
        TARGET_NAME "${${prefix}_TARGET_NAME}"
        STRINGIFY_TOOL_TARGET_NAME "${${prefix}_STRINGIFY_TOOL_TARGET_NAME}"
        STRINGIFY_TOOL_PATH "${${prefix}_STRINGIFY_TOOL_PATH}"
        ARGUMENT_SETS
            "${vertex_shader_arguments}"
            "${fragment_shader_arguments}"
            "${rubiks_cube_model_arguments}"
            "${rubiks_cube_material_library_arguments}"
    )
endfunction()
