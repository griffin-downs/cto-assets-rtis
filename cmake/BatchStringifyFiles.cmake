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
            OUTPUT_DIRECTORY
        MULTI_VALUE_PARAMETERS
            INPUT_FILES
        ARGN ${ARGN}
    )

    if(CMAKE_HOST_WIN32)
        set(exe ".exe")
    else()
        set(exe "")
    endif()

    set(batch_stringify_files
        "${TOOLS_BINARY_DIRECTORY}/BatchStringifyFiles${exe}"
    )

    # set(output_files)
    set(arguments_file "${CMAKE_BINARY_DIR}/BatchStringifyFilesArguments.txt")

    file(WRITE "${arguments_file}"
        "OUTPUT_DIRECTORY=${${prefix}_OUTPUT_DIRECTORY}\n"
    )
    foreach(input_file IN LISTS ${prefix}_INPUT_FILES)
        file(APPEND "${arguments_file}" "INPUT_FILE=${input_file}\n")
    endforeach()

    set(output_file "${CMAKE_BINARY_DIR}/StringifyFilesMarker.txt")
    add_custom_command(
        OUTPUT "${output_file}"
        COMMAND
            "${batch_stringify_files}" < "${arguments_file}"
                > "${output_file}"
        DEPENDS
            "${arguments_file}"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    set(target_name BatchStringifyFiles)
    add_custom_target(${target_name} ALL
        DEPENDS "${output_file}"
        COMMENT "CUSTOM TARGET FOR ${output_file}"
    )
    add_dependencies("${${prefix}_TARGET_NAME}" ${target_name})

    # foreach(output_file IN LISTS output_files)
    #     set(target_name batch_stringify_${output_file})

    #     add_custom_target(${target_name} ALL
    #         DEPENDS "${output_file}"
    #         COMMENT "CUSTOM TARGET FOR ${output_file}"
    #     )

    #     add_dependencies("${${prefix}_TARGET_NAME}" ${target_name})
    # endforeach()
endfunction()
