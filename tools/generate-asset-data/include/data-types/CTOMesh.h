// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>


namespace ctoAssetsRTIS
{
struct CTOMesh
{
    struct Vertex
    {
        std::array<float, 3> position;
        std::optional<std::array<float, 2>> texture;
        std::optional<std::array<float, 3>> normal;
    };

    struct Face
    {
        std::array<unsigned, 3> vertexIndices;
        std::optional<std::array<unsigned, 3>> textureIndices;
    };

    std::string name;
    std::string materialName;

    std::vector<Vertex> vertices;
    std::vector<Face> faces;
};
} // namespace ctoAssetsRTIS
