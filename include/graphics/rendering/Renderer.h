// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include "FragmentShader.glsl.h"
#include "VertexShader.glsl.h"
#include "simulation/FixedRateTimer.h"
#include "simulation/SimulationObject.h"
#include "Shader.h"


namespace ctoAssetsRTIS
{
class Renderer
{
public:
    struct RendererConfiguration
    {
        ProjectionMatrixManager& projectionMatrixManager;
    };
    Renderer(RendererConfiguration configuration)
    : projectionMatrixManager{ configuration.projectionMatrixManager }
    {
        this->shader.use();

        constexpr auto radius = 20.0f;
        const auto view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, radius),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));

        this->shader.set("view", view);

        this->shader.set(
            "projection",
            this->projectionMatrixManager.getMatrix());
    }

    void render(std::span<const SimulationObject> simulationObjects)
    {
        if (this->projectionMatrixManager.wasUpdated())
        {
            this->shader.set(
                "projection",
                this->projectionMatrixManager.getMatrix());
        }

        const auto drawModel =
        [&](const auto& model)
        {
            const auto meshContext = model.mesh.bind();
            for (const auto& materialChunk : model.mesh.materialChunks)
            {
                const auto& materialDefinition =
                    model.materialLibrary.find(materialChunk.name);

                const auto& diffuseColor = materialDefinition.diffuseColor;
                this->shader.set(
                    "color",
                    glm::vec4(
                        diffuseColor[0],
                        diffuseColor[1],
                        diffuseColor[2],
                        1.0f));

                glDrawElements(
                    GL_TRIANGLES,
                    materialChunk.count,
                    GL_UNSIGNED_INT,
                    (void*)(materialChunk.offset * sizeof(GLuint)));
            }
        };

        for (const auto& simulationObject : simulationObjects)
        {
            this->shader.set(
                "model",
                simulationObject
                    .transform
                    .getModelMatrix());

            drawModel(simulationObject.model);
        }
    }

private:
    const Shader shader =
        Shader(
            Shader::SourcePaths
            {
                .vertex = fileContents::VertexShaderGlsl::value.data,
                .fragment = fileContents::FragmentShaderGlsl::value.data
            });

    ProjectionMatrixManager& projectionMatrixManager;
};
} // namespace ctoAssetsRTIS
