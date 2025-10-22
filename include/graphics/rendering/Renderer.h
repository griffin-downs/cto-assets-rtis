// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include "BaseFragmentShader.glsl.h"
#include "BaseVertexShader.glsl.h"
#include "SsaaPass.h"
#include "Shader.h"
#include "simulation/SimulationObject.h"


namespace ctoAssetsRTIS
{
#ifdef DEBUG
namespace
{
    void debugMessageCallback(
        GLenum,
        GLenum,
        GLuint,
        GLenum,
        GLsizei,
        const GLchar*,
        const void*);
} //unnamed namespace
#endif

class Renderer
{
public:
    Renderer(ProjectionMatrixManager& projectionMatrixManager)
    : projectionMatrixManager{ projectionMatrixManager }
    , shaderBase(
        Shader::SourcePaths
        {
            .vertex   = fileContents::BaseVertexShaderGlsl::value.data,
            .fragment = fileContents::BaseFragmentShaderGlsl::value.data
        })
    , SsaaPass(projectionMatrixManager.getViewportDimensions())
    {
#ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(::debugMessageCallback, nullptr);
#endif
        constexpr auto cameraRadius = 20.0f;
        const auto viewMatrix =
            glm::lookAt(
                glm::vec3{ 0.0f, 0.0f, cameraRadius },
                glm::vec3{ 0.0f, 0.0f, 0.0f },
                glm::vec3{ 0.0f, 1.0f, 0.0f });

        for (const auto& shaderProgram : { &this->shaderBase })
        {
            shaderProgram->use();
            shaderProgram->set("view", viewMatrix);
            shaderProgram->set(
                "projection",
                this->projectionMatrixManager.getMatrix());
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDepthMask(GL_TRUE);
    }

    void render(std::span<const SimulationObject> simulationObjects)
    {
        const auto viewportDimensions =
            this->projectionMatrixManager.getViewportDimensions();

        if (this->projectionMatrixManager.pollUpdated())
        {
            shaderBase.use();
            shaderBase.set(
                "projection",
                this->projectionMatrixManager.getMatrix());

            SsaaPass.resize(viewportDimensions);
        }

        const auto SsaaFramebufferContext =
            SsaaPass.beginScene(viewportDimensions);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        // glFrontFace(GL_CCW);

        glDisable(GL_BLEND);

        this->shaderBase.use();

        for (const auto& simulationObject : simulationObjects)
        {
            const auto& [
                objectName,
                model,
                transform,
                angularMotionController
            ] = simulationObject;

            this->shaderBase.set("model", transform.getModelMatrix());

            const auto& [mesh, materialLibrary] = model;
            const auto meshContext = mesh.bind();

            for (const auto& materialChunk : mesh.getMaterialChunks())
            {
                const auto& [materialName, offset, count] = materialChunk;
                const auto& material = materialLibrary.find(materialName);

                this->shaderBase.set(
                    "uColor",
                    glm::vec3
                    {
                        material.diffuseColor[0],
                        material.diffuseColor[1],
                        material.diffuseColor[2]
                    });

                glDrawElements(GL_TRIANGLES,
                            count,
                            GL_UNSIGNED_INT,
                            reinterpret_cast<void*>(offset * sizeof(GLuint)));
            }
        }

        SsaaPass.render(viewportDimensions);
    }

private:
    Shader shaderBase;
    SsaaPass SsaaPass;

    ProjectionMatrixManager& projectionMatrixManager;
};

#ifdef DEBUG
namespace
{
    debugMessageCallback(
        GLenum /* source */,
        GLenum type,
        GLuint /* id */,
        GLenum severity,
        GLsizei /* length */,
        const GLchar* message,
        const void* /* userParam */)
    {
        fprintf(stderr,
                "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                type,
                severity,
                message);
    }
} // unnamed namespace
#endif
} // namespace ctoAssetsRTIS
