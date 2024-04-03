// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
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
    };

    std::vector<Definition> definitions;
};
} // namespace ctoAssetsRTIS
