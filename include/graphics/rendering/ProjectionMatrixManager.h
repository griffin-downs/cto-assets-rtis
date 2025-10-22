// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "window/ViewportDimensions.h"


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
    : matrix{ calculate(this->viewportDimensions) }
    {}

    ProjectionMatrixManager(const ProjectionMatrixManager&) = delete;
    ProjectionMatrixManager(ProjectionMatrixManager&&) = delete;
    ProjectionMatrixManager& operator=(const ProjectionMatrixManager&) = delete;

    const auto& getMatrix() const { return this->matrix; }

    auto getViewportDimensions() const
    {
        return this->viewportDimensions;
    }

    auto pollUpdated()
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
    ViewportDimensions viewportDimensions{ ViewportDimensions::getDefault() };
    glm::mat4 matrix;
    bool updated{ true };
};
} // namespace ctoAssetsRTIS
