# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


function(parse_and_validate_arguments)
    set(PREFIX _PARSE_ARGS)
    set(OPTIONS)
    set(SINGLE_VALUE_PARAMETERS PREFIX)
    set(MULTI_VALUE_PARAMETERS
        OPTIONS
        SINGLE_VALUE_PARAMETERS
        MULTI_VALUE_PARAMETERS
        ARGN
    )

    cmake_parse_arguments(
        _PARSE_ARGS
        "${OPTIONS}"
        "${SINGLE_VALUE_PARAMETERS}"
        "${MULTI_VALUE_PARAMETERS}"
        ${ARGN}
    )

    if(NOT DEFINED _PARSE_ARGS_PREFIX OR NOT _PARSE_ARGS_PREFIX)
        message(FATAL_ERROR "PREFIX argument empty or not provided.")
    endif()

    if(NOT DEFINED _PARSE_ARGS_ARGN)
        message(FATAL_ERROR "ARGN argument not provided.")
    endif()

    cmake_parse_arguments(
        "${_PARSE_ARGS_PREFIX}"
        "${_PARSE_ARGS_OPTIONS}"
        "${_PARSE_ARGS_SINGLE_VALUE_PARAMETERS}"
        "${_PARSE_ARGS_MULTI_VALUE_PARAMETERS}"
        "${_PARSE_ARGS_ARGN}"
    )

    foreach(argument
        IN LISTS
            _PARSE_ARGS_SINGLE_VALUE_PARAMETERS
            _PARSE_ARGS_MULTI_VALUE_PARAMETERS
    )
        if(NOT DEFINED ${_PARSE_ARGS_PREFIX}_${argument})
            message(
                FATAL_ERROR
                "Mandatory argument '${argument}' not provided.")
        endif()
    endforeach()

    foreach(argument
        IN LISTS
            _PARSE_ARGS_SINGLE_VALUE_PARAMETERS
            _PARSE_ARGS_MULTI_VALUE_PARAMETERS
    )
        set(${_PARSE_ARGS_PREFIX}_${argument}
            "${${_PARSE_ARGS_PREFIX}_${argument}}"
            PARENT_SCOPE
        )
    endforeach()
endfunction()
