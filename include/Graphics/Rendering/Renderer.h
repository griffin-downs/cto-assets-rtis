// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include "FragmentShader.h"
#include "Shader.h"
#include "Simulation/FixedRateTimer.h"
#include "Simulation/SimulationObject.h"
#include "VertexShader.h"


namespace ctoAssetsRTIS
{
class Renderer
{
public:
    struct RendererConfiguration
    {
        Camera& camera;
        ProjectionMatrixManager& projectionMatrixManager;
    };
    Renderer(RendererConfiguration configuration)
    : camera{ configuration.camera }
    , projectionMatrixManager{ configuration.projectionMatrixManager }
    {
        this->shader.use();
        this->shader.set(
            "projection",
            this->projectionMatrixManager.getMatrix());
    }

    void render(std::span<const SimulationObject> simulationObjects)
    {
        this->shader.set("view", this->camera.getViewMatrix());

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
                .vertex = fileContents::VertexShaderCode::value.data,
                .fragment = fileContents::FragmentShaderCode::value.data
            });

    Camera& camera;
    ProjectionMatrixManager& projectionMatrixManager;
};
} // namespace ctoAssetsRTIS
