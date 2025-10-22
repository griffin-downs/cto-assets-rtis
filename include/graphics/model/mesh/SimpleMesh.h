// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <span>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "graphics/rendering/GlGeometryBuffers.h"


namespace ctoAssetsRTIS
{
class SimpleMesh
{
public:
    SimpleMesh(CpuGeometry cpuGeometry)
    : glGeometryBuffers{ cpuGeometry }
    , indexCount{ cpuGeometry.faceIndices.size() }
    {
    }

    SimpleMesh(const SimpleMesh&) = delete;
    SimpleMesh(SimpleMesh&&) = delete;
    SimpleMesh& operator=(const SimpleMesh&) = delete;

    auto bind() const
    {
        return this->glGeometryBuffers.bind();
    }

    auto getIndexCount() const
    {
        return this->indexCount;
    }

private:
    const GlGeometryBuffers glGeometryBuffers;
    const size_t indexCount;
};
} // namespace ctoAssetsRTIS
