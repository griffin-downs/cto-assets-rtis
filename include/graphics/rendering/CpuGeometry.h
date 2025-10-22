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


namespace ctoAssetsRTIS
{
struct CpuGeometry
{
    const std::span<const GLfloat> vertices;
    const std::span<const GLuint> faceIndices;
    const std::span<const VertexAttribute> vertexAttributes;
    const GLuint vertexStride;
};
} // namespace ctoAssetsRTIS
