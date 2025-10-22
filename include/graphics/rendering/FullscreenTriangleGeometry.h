// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>
#include <span>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Vertex.h"
#include "CpuGeometry.h"

namespace ctoAssetsRTIS
{
struct FullscreenTriangleGeometry
{
private:
    using VertexType = Vertex<Position2D>;

    static constexpr auto vertices =
        flattenVertices(
            std::to_array<VertexType>({
                VertexType{ Position2D{ -1.0f, -1.0f } },
                VertexType{ Position2D{  3.0f, -1.0f } },
                VertexType{ Position2D{ -1.0f,  3.0f } }
            }));

    static constexpr auto faceIndices = std::to_array<GLuint>({ 0, 1, 2 });

public:
    static constexpr auto value =
        CpuGeometry
        {
            .vertices         = vertices,
            .faceIndices      = faceIndices,
            .vertexAttributes = VertexType::attributes,
            .vertexStride     = VertexType::stride
        };
};
} // namespace ctoAssetsRTIS
