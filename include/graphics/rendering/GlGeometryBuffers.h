// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <span>

#include "GlObject.h"
#include "CpuGeometry.h"

namespace ctoAssetsRTIS
{
class GlGeometryBuffers
{
public:
    GlGeometryBuffers(CpuGeometry cpuGeometry)
    {
        const auto vaoContext = this->vertexArray.bind();
        const auto vboContext = this->vertexBuffer.bind();
        const auto eboContext = this->elementBuffer.bind();

        vboContext.bufferData(cpuGeometry.vertices);
        eboContext.bufferData(cpuGeometry.faceIndices);
        vaoContext.configureAttributes(
            cpuGeometry.vertexStride,
            cpuGeometry.vertexAttributes);
    }

    GlGeometryBuffers(const GlGeometryBuffers&) = delete;
    GlGeometryBuffers(GlGeometryBuffers&&) = delete;
    GlGeometryBuffers& operator=(const GlGeometryBuffers&) = delete;

    class Context
    {
    private:
        const GlVertexArray::Context vaoContext;
        const GlVertexBuffer::Context vboContext;
        const GlElementBuffer::Context eboContext;

    public:
        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(const Context&) = delete;

        Context(const GlGeometryBuffers& glGeometryBuffers)
        : vaoContext{ glGeometryBuffers.vertexArray.bind() }
        , vboContext{ glGeometryBuffers.vertexBuffer.bind() }
        , eboContext{ glGeometryBuffers.elementBuffer.bind() }
        {
        }
    };

    auto bind() const { return Context(*this); }

private:
    const GlVertexArray vertexArray;
    const GlVertexBuffer vertexBuffer;
    const GlElementBuffer elementBuffer;

    friend class Context;
};
} // namespace ctoAssetsRTIS
