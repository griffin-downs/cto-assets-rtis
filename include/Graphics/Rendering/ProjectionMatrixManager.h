// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window/ViewportDimensions.h"


namespace ctoAssetsRTIS
{
class ProjectionMatrixManager
{
private:
    static auto calculate(ViewportDimensions viewportDimensions)
    {
        const auto [width, height] = viewportDimensions;

        return
            glm::perspective(
                glm::radians(45.0f),
                (float)width / (float)height,
                0.5f,
                100.0f);
    }

public:
    ProjectionMatrixManager()
    : viewportDimensions{ ViewportDimensions::getDefault() }
    , matrix{ calculate(this->viewportDimensions) }
    , updated{ true }
    {
    }

    ProjectionMatrixManager(const ProjectionMatrixManager&) = delete;
    ProjectionMatrixManager(ProjectionMatrixManager&&) = delete;
    ProjectionMatrixManager& operator=(const ProjectionMatrixManager&) = delete;

    const auto& getMatrix() const { return this->matrix; }

    auto getViewportDimensions() const
    {
        return this->viewportDimensions;
    }

    bool wasUpdated()
    {
        if (!this->updated)
        {
            return false;
        }

        this->updated = false;
        return true;
    }

    void setViewportDimensions(ViewportDimensions viewportDimensions)
    {
        this->viewportDimensions = viewportDimensions;
        this->matrix = calculate(viewportDimensions);
        this->updated = true;
    }

private:
    ViewportDimensions viewportDimensions;
    glm::mat4 matrix;
    bool updated;
};
} // namespace ctoAssetsRTIS
