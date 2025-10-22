// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>
#include <string>
#include <vector>


namespace ctoAssetsRTIS
{
struct CTOMaterialLibrary
{
    struct Definition
    {
        std::string name;
        std::array<float, 3> diffuseColor;
        float opacity;
    };

    std::vector<Definition> definitions;
};
} // namespace ctoAssetsRTIS
