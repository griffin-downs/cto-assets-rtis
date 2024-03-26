# ==============================================================================
# Copyright (C) 2024, Griffin Downs. All rights reserved.
# This file is part of cto-assets-rtis. See LICENSE.md for details.
# ==============================================================================


function(make_named_tuple)
    set(prefix _TUPLE)
    parse_and_validate_arguments(
        PREFIX ${prefix}
        SINGLE_VALUE_PARAMETERS
            OUTPUT_VARIABLE
        MULTI_VALUE_PARAMETERS
            PARAMETER_SET
            ARGUMENTS
        ARGN ${ARGN}
    )

    list(LENGTH ${prefix}_PARAMETER_SET parameter_set_length)
    list(LENGTH ${prefix}_ARGUMENTS arguments_length)
    math(EXPR expected_arguments_length "${parameter_set_length} * 2")

    if(NOT arguments_length EQUAL expected_arguments_length)
        message(
            FATAL_ERROR
            "ARGUMENTS does not have the expected size. "
            "Each parameter must have a corresponding value."
        )
    endif()

    set(tuple)

    foreach(parameter IN LISTS ${prefix}_PARAMETER_SET)
        list(FIND ${prefix}_ARGUMENTS ${parameter} parameter_index)

        if(parameter_index EQUAL -1)
            message(
                FATAL_ERROR
                "Validation failed: Parameter '${parameter}' not found."
            )
        endif()

        math(EXPR value_index "${parameter_index} + 1")
        list(GET ${prefix}_ARGUMENTS ${value_index} value)

        set(key_value_pair "${parameter}=${value}")
        list(APPEND tuple "${key_value_pair}")
    endforeach()

    set("${${prefix}_OUTPUT_VARIABLE}" "${tuple}" PARENT_SCOPE)
endfunction()
