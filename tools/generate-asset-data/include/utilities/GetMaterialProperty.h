// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <exception>
#include <format>

#include <assimp/material.h>

#include "converters/AssimpSTLConverters.h"


namespace ctoAssetsRTIS
{
template<typename T, typename... Args>
auto getMaterialProperty(const aiMaterial& material, Args&&... args)
{
    static T outVariable;

    auto returnCode = material.Get(std::forward<Args>(args)..., outVariable);
    if (returnCode != aiReturn_SUCCESS)
    {
        throw
            std::runtime_error(
                std::format(
                    "Failed to retrieve property:\n"
                    "\tRequested Type: {}",
                    typeid(T).name()));
    }

    return assimpSTLConverters::convert(outVariable);
}
} // namespace ctoAssetsRTIS
