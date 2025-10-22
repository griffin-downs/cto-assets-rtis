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

    set(tool "${TOOLS_BINARY_DIRECTORY}/BatchStringifyFiles${exe}")
    set(args_file "${CMAKE_BINARY_DIR}/BatchStringifyFilesArguments.txt")
    set(manifest  "${CMAKE_BINARY_DIR}/BatchStringifyFilesOutput.txt")

    # Build the list of echo-append commands that write the args file
    set(_cmds)
    list(APPEND _cmds
        COMMAND "${CMAKE_COMMAND}" -E rm -f "${args_file}"
        COMMAND "${CMAKE_COMMAND}" -E echo "OUTPUT_DIRECTORY=${${prefix}_OUTPUT_DIRECTORY}" > "${args_file}"
    )
    foreach(f IN LISTS ${prefix}_INPUT_FILES)
        list(APPEND _cmds
            COMMAND "${CMAKE_COMMAND}" -E echo "INPUT_FILE=${f}" >> "${args_file}")
    endforeach()

    add_custom_command(
        OUTPUT "${manifest}"
        ${_cmds}
        COMMAND "${tool}" < "${args_file}" > "${manifest}"
        DEPENDS ${${prefix}_INPUT_FILES}
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        COMMENT "BatchStringifyFiles → ${manifest}"
        VERBATIM
    )

    add_custom_target(BatchStringifyFilesOutput
        DEPENDS "${manifest}"
    )
    add_dependencies("${${prefix}_TARGET_NAME}" BatchStringifyFilesOutput)
endfunction()
