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

#include "Vertex.h"

namespace ctoAssetsRTIS
{
template<
    typename ContextType,
    auto CreateFunction,
    auto DeleteFunction
>
class GLObject
{
public:
    using Context = ContextType;

    GLObject() { CreateFunction(1, &this->id); }

    GLObject(const GLObject&) = delete;
    GLObject(GLObject&&) = delete;
    GLObject& operator=(const GLObject&) = delete;

    ~GLObject() { DeleteFunction(1, &this->id); }

    auto bind() const { return Context(this->id); }

private:
    GLuint id;
};

template<typename TypeTag = void, GLenum EnumTag = 0>
struct GetGLObject;

template<GLenum BufferType>
struct GetGLObject<void, BufferType>
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
        GLObject<
            Context,
            [](GLsizei n, GLuint* id) { glGenBuffers(n, id); },
            [](GLsizei n, GLuint* id) { glDeleteBuffers(n, id); }
        >;
};

class GLVertexArray;

template<>
struct GetGLObject<GLVertexArray>
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
        GLObject<
            Context,
            [](GLsizei n, GLuint* id) { glGenVertexArrays(n, id); },
            [](GLsizei n, GLuint* id) { glDeleteVertexArrays(n, id); }
        >;
};

class GLVertexArray
    : public GetGLObject<GLVertexArray>::Type {};

class GLVertexBuffer
    : public GetGLObject<void, GL_ARRAY_BUFFER>::Type {};

class GLElementBuffer
    : public GetGLObject<void, GL_ELEMENT_ARRAY_BUFFER>::Type {};
} // namespace ctoAssetsRTIS
