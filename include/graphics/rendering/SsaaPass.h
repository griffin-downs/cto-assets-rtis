// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include "FullscreenTriangleGeometry.h"
#include "SsaaFragmentShader.glsl.h"
#include "SsaaVertexShader.glsl.h"
#include "graphics/model/mesh/SimpleMesh.h"
#include "graphics/rendering/GlObject.h"
#include "Shader.h"
#include "window/ViewportDimensions.h"

namespace ctoAssetsRTIS
{
class SsaaPass
{
private:
    static auto scale(ViewportDimensions viewportDimensions)
    {
        const auto scale =
        [](int value)
        {
            constexpr auto scaleFactor = 4.0f;
            return static_cast<int>(scaleFactor * value + 0.5f);
        };

        return
            ViewportDimensions({
                .width = scale(viewportDimensions.getWidth()),
                .height = scale(viewportDimensions.getHeight())
            });
    }

public:
    SsaaPass(ViewportDimensions viewportDimensions)
    : triangleMesh(SimpleMesh(FullscreenTriangleGeometry::value))
    , shader(
        Shader::SourcePaths
        {
            .vertex   = fileContents::SsaaVertexShaderGlsl::value.data,
            .fragment = fileContents::SsaaFragmentShaderGlsl::value.data
        })
    {
        const auto [width, height] = scale(viewportDimensions);
        {
            const auto context = this->colorTexture.bind();
            context.storageRGBA8(width, height);
            context.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
            context.setMagFilter(GL_LINEAR);
            context.setWrapS(GL_CLAMP_TO_EDGE);
            context.setWrapT(GL_CLAMP_TO_EDGE);
        }
        {
            const auto context = this->renderbuffer.bind();
            context.storageDepth24(width, height);
        }
        {
            const auto context = this->framebuffer.bind();
            glFramebufferTexture2D(
                GL_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_2D,
                this->colorTexture.getId(),
                0);
            glFramebufferRenderbuffer(
                GL_FRAMEBUFFER,
                GL_DEPTH_ATTACHMENT,
                GL_RENDERBUFFER,
                this->renderbuffer.getId());

#ifndef __EMSCRIPTEN__
            const GLenum bufs[1] = { GL_COLOR_ATTACHMENT0 };
            glDrawBuffers(1, bufs);
#endif

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
                != GL_FRAMEBUFFER_COMPLETE)
            {
                throw std::runtime_error("Ssaa FBO incomplete");
            }
        }

        this->shader.use();
        this->shader.set("uScene", 0);
        this->shader.set(
            "uInverseResolution",
            glm::vec2
            {
                1.0f / width,
                1.0f / height
            });
    }

    auto beginScene(ViewportDimensions viewportDimensions) const
    {
        const auto [width, height] = scale(viewportDimensions);
        auto context = this->framebuffer.bind();

        glViewport(0, 0, width, height);
        glClearColor(0.07f, 0.08f, 0.11f, 1.0f);
        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT |
            GL_STENCIL_BUFFER_BIT);

        return context;
    }

    void resize(ViewportDimensions viewportDimensions)
    {
        const auto [width, height] = scale(viewportDimensions);
        {
            const auto context = this->colorTexture.bind();
            context.storageRGBA8(width, height);
        }
        {
            const auto context = this->renderbuffer.bind();
            context.storageDepthStencil(width, height);
        }
        {
            const auto context = this->framebuffer.bind();
            if (!context.checkComplete())
            {
                throw std::runtime_error("Ssaa FBO incomplete after resize");
            }
        }

        this->shader.use();
        this->shader.set(
            "uInverseResolution",
            glm::vec2
            {
                1.0f / width,
                1.0f / height
            });
    }

    void render(ViewportDimensions viewportDimensions) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
     
        const auto [width, height] = viewportDimensions;
        glViewport(0, 0, width, height);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        this->shader.use();

        const auto textureContext = this->colorTexture.bind();
        textureContext.generateMipmap();
        textureContext.bindToUnit(0);

        const auto meshContext = this->triangleMesh.bind();
        glDrawElements(
            GL_TRIANGLES,
            this->triangleMesh.getIndexCount(),
            GL_UNSIGNED_INT,
            nullptr);
    }

private:
    GlFramebuffer framebuffer;
    GlTexture2D colorTexture;
    GlRenderbuffer renderbuffer;

    SimpleMesh triangleMesh;
    Shader shader;
};
} // namespace ctoAssetsRTIS
