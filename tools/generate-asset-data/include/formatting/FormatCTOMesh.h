// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <ostream>

#include "FormatContainer.h"
#include "data-types/CTOMesh.h"


namespace ctoAssetsRTIS
{
using namespace spaceDelimited::array;
using namespace newlineDelimited::vector;

auto& operator<<(std::ostream& ostream, const CTOMesh::Vertex& vertex)
{
    const auto& [
        position,
        texture,
        normal
    ] = vertex;

    const auto hasTexture = vertex.texture.has_value();
    const auto hasNormal = vertex.normal.has_value();

    const auto identifier =
    [&]
    {
        if (hasTexture && hasNormal)
        {
            return "vptn";
        }
        if (hasTexture)
        {
            return "vpt";
        }
        if (hasNormal)
        {
            return "vpn";
        }

        return "vp";
    }();

    ostream << identifier << ' ' << position;
    if (hasTexture)
    {
        ostream << ' ' << texture.value();
    }
    if (hasNormal)
    {
        ostream << ' ' << normal.value();
    }

    return ostream;
}

auto& operator<<(std::ostream& ostream, const CTOMesh::Face& face)
{
    const auto& [
        vertexIndices,
        textureIndices
    ] = face;

    const auto printElement =
    [
        &,
        hasTextureIndices = textureIndices.has_value()
    ](size_t index)
    {
        ostream << vertexIndices[index];

        if (hasTextureIndices)
        {
            ostream << '/' << textureIndices.value()[index];
        }
    };

    ostream << "f ";

    const auto lastIndex = vertexIndices.size() - 1;
    for (auto i = size_t{}; i < lastIndex; i++)
    {
        printElement(i);
        ostream << ' ';
    }

    printElement(lastIndex);

    return ostream;
}

auto& operator<<(std::ostream& ostream, const CTOMesh& ctoMesh)
{
    const auto& [
        name,
        materialName,
        vertices,
        faces
    ] = ctoMesh;

    return
        ostream
            << "o " << name << '\n'
            << vertices << '\n'
            << "usemtl " << materialName << '\n'
            << faces;
}
} // namespace ctoAssetsRTIS