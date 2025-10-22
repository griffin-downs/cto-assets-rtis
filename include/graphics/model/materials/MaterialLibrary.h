// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <algorithm>
#include <array>
#include <format>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace ctoAssetsRTIS
{
class MaterialLibrary
{
public:
    struct Definition
    {
        std::string_view name;
        std::array<float, 3> diffuseColor;
        float opacity;
    };

    constexpr MaterialLibrary(std::span<const Definition> materialDefinitions)
    : materialDefinitions{ materialDefinitions }
    {
    }

    const auto& find(std::string_view materialName) const
    {
        const auto materialDefinitionsIterator =
            std::find_if(
                materialDefinitions.begin(),
                materialDefinitions.end(),
                [&](const auto& materialDefinition)
                {
                    return materialDefinition.name == materialName;
                });

        if (materialDefinitionsIterator == materialDefinitions.end())
        {
            throw
                std::runtime_error(
                    std::format("Cannot find material: {}", materialName));
        }

        return *materialDefinitionsIterator;
    }

private:
    const std::span<const Definition> materialDefinitions;
};
} // namespace ctoAssetsRTIS
