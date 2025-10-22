// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>

#include "graphics/model/materials/MaterialLibrary.h"


namespace ctoAssetsRTIS
{
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
                            .diffuseColors[0],
                    .opacity =
                        std::get<Indices>(parsedMaterialDefinitions)
                            .opacities[0][0]
                }), ...);
        }(std::make_index_sequence<materialDefinitionsCount>{});

        return result;
    }();

public:
    static constexpr auto value = MaterialLibrary(materialDefinitions);
};
} // namespace ctoAssetsRTIS
