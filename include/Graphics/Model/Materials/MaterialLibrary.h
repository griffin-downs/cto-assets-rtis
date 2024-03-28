// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
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

template<typename StringProvider>
class CompileTimeDeserialize<MaterialLibrary, StringProvider>
{
private:
    static constexpr auto& parsedMaterialDefinitions =
        ParseWavefrontMtl<StringProvider>::value;

    static constexpr auto materialDefinitions =
    []
    {
        constexpr auto materialDefinitionsCount =
            std::tuple_size<
                typename std::decay<decltype(parsedMaterialDefinitions)>::type
            >::value;

        auto result =
            std::array<
                MaterialLibrary::Definition,
                materialDefinitionsCount
            >{};

        static constexpr auto idStringCache =
            std::apply(
                [](auto&&... materialDefinitions)
                {
                    return
                        std::make_tuple(
                            AutomaticDurationString(materialDefinitions.id)...);
                },
                parsedMaterialDefinitions);

        [&]<size_t... Indices>(std::index_sequence<Indices...>)
        {
            ((result[Indices] =
                MaterialLibrary::Definition
                {
                    .name =
                        std::get<Indices>(idStringCache).toStringView(),
                    .diffuseColor =
                        std::get<Indices>(parsedMaterialDefinitions)
                            .diffuseColors[0]
                }), ...);
        }(std::make_index_sequence<materialDefinitionsCount>{});

        return result;
    }();

public:
    static constexpr auto value = MaterialLibrary(materialDefinitions);
};
} // namespace ctoAssetsRTIS
