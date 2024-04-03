// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <GL/glew.h>
#endif

#include "Graphics/Model/Mesh/Vertex.h"

namespace ctoAssetsRTIS
{
template<
    typename ContextType,
    auto CreateFunction,
    auto DeleteFunction
>
class GlObject
{
public:
    using Context = ContextType;

    GlObject() { CreateFunction(1, &this->id); }

    GlObject(const GlObject&) = delete;
    GlObject(GlObject&&) = delete;
    GlObject& operator=(const GlObject&) = delete;

    ~GlObject() { DeleteFunction(1, &this->id); }

    auto bind() const { return Context(this->id); }

private:
    GLuint id;
};

template<typename TypeTag = void, GLenum EnumTag = 0>
struct GetGlObject;

template<GLenum BufferType>
struct GetGlObject<void, BufferType>
{
    class Context
    {
    public:
        Context(GLuint id) { glBindBuffer(BufferType, id); }
        ~Context() { glBindBuffer(BufferType, 0); }

        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(const Context&) = delete;

        template<typename T>
        void bufferData(std::span<const T> span) const
        {
            glBufferData(
                BufferType,
                span.size_bytes(),
                span.data(),
                GL_STATIC_DRAW);
        }
    };

    using Type =
        GlObject<
            Context,
            [](GLsizei n, GLuint* id) { glGenBuffers(n, id); },
            [](GLsizei n, GLuint* id) { glDeleteBuffers(n, id); }
        >;
};

class GlVertexArray;

template<>
struct GetGlObject<GlVertexArray>
{
    class Context
    {
    public:
        Context(GLuint id) { glBindVertexArray(id); }
        ~Context() { glBindVertexArray(0); }

        Context(const Context&) = delete;
        Context(const Context&&) = delete;
        Context& operator=(const Context&) = delete;

        void configureAttributes(
            size_t vertexStride,
            std::span<const VertexAttribute> vertexAttributes) const
        {
            for (const auto& attribute : vertexAttributes)
            {
                auto [index, elementCount, elementType, offset] = attribute;

                glVertexAttribPointer(
                    index,
                    elementCount,
                    elementType,
                    GL_FALSE,
                    static_cast<GLsizei>(vertexStride),
                    reinterpret_cast<const void*>(offset));

                glEnableVertexAttribArray(index);
            }
        }
    };

    using Type =
        GlObject<
            Context,
            [](GLsizei n, GLuint* id) { glGenVertexArrays(n, id); },
            [](GLsizei n, GLuint* id) { glDeleteVertexArrays(n, id); }
        >;
};

class GlVertexArray
    : public GetGlObject<GlVertexArray>::Type {};

class GlVertexBuffer
    : public GetGlObject<void, GL_ARRAY_BUFFER>::Type {};

class GlElementBuffer
    : public GetGlObject<void, GL_ELEMENT_ARRAY_BUFFER>::Type {};
} // namespace ctoAssetsRTIS
