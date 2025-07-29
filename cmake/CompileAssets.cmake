# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================

include("${CMAKE_SOURCE_DIR}/cmake/BatchStringifyFiles.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/MakeNamedTuple.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ParseAndValidateArguments.cmake")

function(compile_assets)
    set(prefix _COMPILE_ASSETS)
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

    set(generate_asset_data
        "${TOOLS_BINARY_DIRECTORY}/GenerateAssetData${exe}"
    )
    set(batch_stringify_files
        "${TOOLS_BINARY_DIRECTORY}/BatchStringifyFiles${exe}"
    )
    # set(compile_asset_library
    #     "${TOOLS_BINARY_DIRECTORY}/CompileAssetLibrary${exe}"
    # )

    set(arguments_file "${CMAKE_BINARY_DIR}/GenerateAssetDataArguments.txt")
    file(WRITE "${arguments_file}"
        "OUTPUT_DIRECTORY=${${prefix}_OUTPUT_DIRECTORY}\n"
    )
    foreach(input_file IN LISTS ${prefix}_INPUT_FILES)
        file(APPEND "${arguments_file}" "INPUT_FILE=${input_file}\n")
    endforeach()

    set(generate_asset_data_output
        "${CMAKE_BINARY_DIR}/GenerateAssetDataOutput.txt"
    )
    set(output_file "${CMAKE_BINARY_DIR}/CompileAssetOutput.txt")

    add_custom_command(
        OUTPUT "${output_file}"
        COMMAND
            "${generate_asset_data}" < "${arguments_file}"
                > "${generate_asset_data_output}"
        COMMAND
            "${batch_stringify_files}" < "${generate_asset_data_output}"
                > "${output_file}"
        DEPENDS "${arguments_file}"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    set(target_name CompileAssetsOutput)
    add_custom_target(${target_name} ALL
        DEPENDS "${output_file}"
    )
    add_dependencies("${${prefix}_TARGET_NAME}" ${target_name})

    
    # execute_process(
    #     ${generate_asset_data} < ${arguments_file}
    #         | ${batch_stringify_files}
    #         | ${compile_asset_library}
    #         > ${output_file})

    # file(READ "${output_file}" compile_asset_library_output)
    # foreach(line IN LISTS compile_asset_library_output)
    #     if(line MATCHES "^LINK_LIBRARY=")
        
    #     elseif(line MATCHES "^INCLUDE_DIRECTORY=")
    #     endif()
    # endforeach()
endfunction()
