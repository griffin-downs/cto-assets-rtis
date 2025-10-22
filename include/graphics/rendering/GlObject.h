// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <span>
#include <cstdint>

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
class GlObject
{
public:
    using Context = ContextType;

    GlObject()
    {
        CreateFunction(1, &this->id);
    }

    GlObject(const GlObject&) = delete;
    GlObject(GlObject&&) = delete;
    GlObject& operator=(const GlObject&) = delete;

    ~GlObject()
    {
        DeleteFunction(1, &this->id);
    }

    auto bind() const
    {
        return Context(this->id);
    }

    auto getId() const
    {
        return this->id;
    }

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

template<>
struct GetGlObject<void, GL_TEXTURE_2D>
{
    class Context
    {
    public:
        Context(GLuint id)
        : id{ id }
        {
            glBindTexture(GL_TEXTURE_2D, this->id);
        }

        ~Context()
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(const Context&) = delete;

        void allocate(
            GLint internalFormat,
            GLsizei width,
            GLsizei height,
            GLenum format,
            GLenum type,
            const void* data = nullptr) const
        {
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                internalFormat,
                width,
                height,
                0,
                format,
                type,
                data);
        }

        void storageRGBA16F(GLsizei width, GLsizei height) const
        {
            this->allocate(
                GL_RGBA16F,
                width,
                height,
                GL_RGBA,
                GL_HALF_FLOAT,
                nullptr);
        }
        void storageRGBA8(GLsizei width, GLsizei height) const
        {
            this->allocate(
                GL_RGBA8,
                width,
                height,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                nullptr);
        }
        void storageR32F(GLsizei width, GLsizei height) const
        {
            this->allocate(
                GL_R32F,
                width,
                height,
                GL_RED,
                GL_FLOAT,
                nullptr);
        }

        void setMinFilter(GLenum v) const
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, v);
        }
        void setMagFilter(GLenum v) const
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, v);
        }
        void setWrapS(GLenum v) const
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, v);
        }
        void setWrapT(GLenum v) const
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, v);
        }

        void generateMipmap() const
        {
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        void bindToUnit(GLuint unit) const
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, this->id);
        }

    private:
        GLuint id;
    };

    using Type =
        GlObject<
            Context,
            [](GLsizei n, GLuint* id) { glGenTextures(n, id); },
            [](GLsizei n, GLuint* id) { glDeleteTextures(n, id); }
        >;
};

template<>
struct GetGlObject<void, GL_RENDERBUFFER>
{
    class Context
    {
    public:
        Context(GLuint id)
        {
            glBindRenderbuffer(GL_RENDERBUFFER, id);
        }

        ~Context()
        {
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }

        Context(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(const Context&) = delete;

        void storageDepthStencil(GLsizei width, GLsizei height) const
        {
            glRenderbufferStorage(
                GL_RENDERBUFFER,
                GL_DEPTH24_STENCIL8,
                width,
                height);
        }

        void storageDepth24(GLsizei width, GLsizei height) const
        {
            glRenderbufferStorage(
                GL_RENDERBUFFER,
                GL_DEPTH_COMPONENT24,
                width,
                height);
        }
    };

    using Type =
        GlObject<
            Context,
            [](GLsizei n, GLuint* id) { glGenRenderbuffers(n, id); },
            [](GLsizei n, GLuint* id) { glDeleteRenderbuffers(n, id); }
        >;
};

template<>
struct GetGlObject<void, GL_FRAMEBUFFER>
{
    class Context
    {
    public:
        Context(GLuint id)
        : id{ id }
        {
            glBindFramebuffer(GL_FRAMEBUFFER, id);
        }

        ~Context()
        {
            if (!this->id)
            {
                return;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        Context(Context&& context)
        : id{ context.id }
        {
            context.id = 0;
        }

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        void attachColor(GLenum attachment, GLuint textureId) const
        {
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                attachment,
                GL_TEXTURE_2D,
                textureId,
                0);
        }
        void attachDepthStencil(GLuint renderbufferId) const
        {
            glFramebufferRenderbuffer(
                GL_FRAMEBUFFER,
                GL_DEPTH_STENCIL_ATTACHMENT,
                GL_RENDERBUFFER,
                renderbufferId);
        }

        auto checkComplete() const
        {
            return
                glCheckFramebufferStatus(GL_FRAMEBUFFER)
                    == GL_FRAMEBUFFER_COMPLETE;
        }

    private:
        GLuint id;
    };

    using Type =
        GlObject<
            Context,
            [](GLsizei n, GLuint* id) { glGenFramebuffers(n, id); },
            [](GLsizei n, GLuint* id) { glDeleteFramebuffers(n, id); }
        >;
};

class GlVertexArray : public GetGlObject<GlVertexArray>::Type {};

class GlVertexBuffer : public GetGlObject<void, GL_ARRAY_BUFFER>::Type {};

class GlElementBuffer
    : public GetGlObject<void, GL_ELEMENT_ARRAY_BUFFER>::Type {};

class GlTexture2D : public GetGlObject<void, GL_TEXTURE_2D>::Type {};

class GlRenderbuffer : public GetGlObject<void, GL_RENDERBUFFER>::Type {};

class GlFramebuffer : public GetGlObject<void, GL_FRAMEBUFFER>::Type {};
} // namespace ctoAssetsRTIS
