// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <span>

#include "Graphics/Rendering/GLObject.h"
#include "MeshData.h"

namespace ctoAssetsRTIS
{
class Mesh
{
public:
    Mesh(MeshData meshData)
    : materialChunks{ meshData.materialChunks }
    {
        const auto vaoContext = this->vertexArray.bind();
        const auto vboContext = this->vertexBuffer.bind();
        const auto eboContext = this->elementBuffer.bind();

        vboContext.bufferData(meshData.vertices);
        eboContext.bufferData(meshData.faceIndices);
        vaoContext.configureAttributes(
            meshData.vertexStride,
            meshData.vertexAttributes);
    }

    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    class Context
    {
    private:
        const GLVertexArray::Context vaoContext;
        const GLVertexBuffer::Context vboContext;
        const GLElementBuffer::Context eboContext;

    public:
        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(const Context&) = delete;

        Context(const Mesh& mesh)
        : vaoContext{ mesh.vertexArray.bind() }
        , vboContext{ mesh.vertexBuffer.bind() }
        , eboContext{ mesh.elementBuffer.bind() }
        {
        }
    };

    auto bind() const { return Context(*this); }

    const std::span<const MeshData::MaterialChunk> materialChunks;

private:
    const GLVertexArray vertexArray;
    const GLVertexBuffer vertexBuffer;
    const GLElementBuffer elementBuffer;

    friend class Context;
};
} // namespace ctoAssetsRTIS
