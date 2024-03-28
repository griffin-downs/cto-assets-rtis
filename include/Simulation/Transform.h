// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Rotation.h"


namespace ctoAssetsRTIS
{
struct Transform
{
    auto getModelMatrix() const
    {
        auto modelMatrix = this->rotation.getRotationMatrix();

        modelMatrix[0] *= this->scale.x;
        modelMatrix[1] *= this->scale.y;
        modelMatrix[2] *= this->scale.z;

        modelMatrix[3][0] = this->position.x;
        modelMatrix[3][1] = this->position.y;
        modelMatrix[3][2] = this->position.z;

        modelMatrix[0][3] = 0.0f;
        modelMatrix[1][3] = 0.0f;
        modelMatrix[2][3] = 0.0f;
        modelMatrix[3][3] = 1.0f;

        return modelMatrix;
    }

    glm::vec3 position = glm::vec3();
    glm::vec3 scale = glm::vec3(1.0f);
    Rotation rotation;
};
} // namespace ctoAssetsRTIS