# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


set(BATCH_STRINGIFY_FILES_CPP_PARAMETER_SET
    INPUT_FILE
    OUTPUT_FILE
    TYPE_NAME
)
function(batch_stringify_files)
    set(prefix _STRINGIFY)
    parse_and_validate_arguments(
        PREFIX ${prefix}
        SINGLE_VALUE_PARAMETERS
            TARGET_NAME
            STRINGIFY_TOOL_TARGET_NAME
            STRINGIFY_TOOL_PATH
        MULTI_VALUE_PARAMETERS
            ARGUMENT_SETS
        ARGN ${ARGN}
    )

    set(output_files)
    set(arguments_file "${CMAKE_BINARY_DIR}/BatchStringifyFilesArguments.txt")

    file(WRITE "${arguments_file}" "")
    foreach(argument IN LISTS ${prefix}_ARGUMENT_SETS)
        file(APPEND "${arguments_file}" "${argument}\n")
        if(argument MATCHES "^OUTPUT_FILE=")
            string(REGEX REPLACE "^OUTPUT_FILE=" "" output_file ${argument})
            list(APPEND output_files ${output_file})
        endif()
    endforeach()

    add_custom_command(
        OUTPUT ${output_files}
        COMMAND "${${prefix}_STRINGIFY_TOOL_PATH}" < "${arguments_file}"
        DEPENDS
            ${${prefix}_STRINGIFY_TOOL_TARGET_NAME}
            "${arguments_file}"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    foreach(output_file IN LISTS output_files)
        set(target_name batch_stringify_${output_file})

        add_custom_target(${target_name} ALL
            DEPENDS "${output_file}"
            COMMENT "CUSTOM TARGET FOR ${output_file}"
        )

        add_dependencies("${${prefix}_TARGET_NAME}" ${target_name})
    endforeach()
endfunction()
