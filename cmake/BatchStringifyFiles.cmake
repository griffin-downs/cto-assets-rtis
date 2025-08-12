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

    set(arguments_file "${CMAKE_BINARY_DIR}/BatchStringifyFilesArguments.txt")
    file(WRITE "${arguments_file}"
        "OUTPUT_DIRECTORY=${${prefix}_OUTPUT_DIRECTORY}\n"
    )
    foreach(input_file IN LISTS ${prefix}_INPUT_FILES)
        file(APPEND "${arguments_file}" "INPUT_FILE=${input_file}\n")
    endforeach()

    set(tool_output_file "${CMAKE_BINARY_DIR}/BatchStringifyFilesOutput.txt")
    execute_process(
        COMMAND "${batch_stringify_files}"
        INPUT_FILE "${arguments_file}"
        OUTPUT_FILE "${tool_output_file}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    set(target_index 0)
    file(STRINGS "${tool_output_file}" tool_output)
    foreach(line IN LISTS tool_output)
        if(line MATCHES "^INPUT_FILE=")
            string(REGEX REPLACE "^INPUT_FILE=" "" line ${line})
        else()
            continue()
        endif()

        set(target_name batch_stringify_${target_index})

        add_custom_target(${target_name} ALL
            DEPENDS "${line}"
            COMMENT "CUSTOM TARGET FOR ${line}"
        )

        add_dependencies("${${prefix}_TARGET_NAME}" ${target_name})

        math(EXPR target_index "${target_index} + 1")
    endforeach()
endfunction()
