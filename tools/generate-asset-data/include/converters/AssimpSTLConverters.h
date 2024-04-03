// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>
#include <string>

#include <assimp/types.h>


namespace ctoAssetsRTIS::assimpSTLConverters
{
auto convert(const aiString& string)
{
    return std::string(string.C_Str());
}

auto convert(const aiColor3D& color)
{
    return std::array<float, 3>{ color.r, color.g, color.b };
}

auto convert(const aiVector3D& vector)
{
    return std::array<float, 3>{ vector.x, vector.y, vector.z };
}
} // namespace ctoAssetsRTIS
