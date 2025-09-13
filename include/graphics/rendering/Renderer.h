// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include "FragmentShader.glsl.h"
#include "VertexShader.glsl.h"
#include "simulation/FixedRateTimer.h"
#include "simulation/SimulationObject.h"
#include "RenderData.h"
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
        const auto view =
            glm::lookAt(
                glm::vec3{ 0.0f, 0.0f, radius },
                glm::vec3{ 0.0f, 0.0f, 0.0f },
                glm::vec3{ 0.0f, 1.0f, 0.0f });

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

        const auto drawSimulationObject =
        [&](const auto& simulationObject)
        {
            const auto& [
                model,
                transform,
                angularMotion
            ] = simulationObject;

            this->shader.set("model", transform.getModelMatrix());
            // this->applyRenderData(model.renderData);

            const auto& [
                mesh,
                materialLibrary,
                renderData
            ] = model;

            this->applyRenderData(renderData);

            const auto meshContext = mesh.bind();
            for (const auto& materialChunk : mesh.materialChunks)
            {
                const auto& [
                    materialName,
                    offset,
                    count
                ] = materialChunk;

                const auto& [
                    _,
                    diffuseColor
                ] = materialLibrary.find(materialName);

                this->shader.set(
                    "color",
                    glm::vec4(
                        diffuseColor[0],
                        diffuseColor[1],
                        diffuseColor[2],
                        1.0f
                ));

                glDrawElements(
                    GL_TRIANGLES,
                    count,
                    GL_UNSIGNED_INT,
                    (void*)(offset * sizeof(GLuint)));
            }
        };

        for (const auto& simulationObject : simulationObjects)
        {
            drawSimulationObject(simulationObject);
        }
    }

private:
    void applyRenderData(const RenderData& renderData)
    {
        // this->shader.set();
        // this->shader.set();
        // this->shader.set();
    }

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
