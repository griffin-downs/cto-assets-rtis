// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace ctoAssetsRTIS
{
class Rotation
{
public:
    Rotation()
    : orientation(1.0f, 0.0f, 0.0f, 0.0f)
    {}

    auto applyDeltaRadians(glm::vec3 deltaRadians)
    {
        const auto globalRight   = glm::vec3{ 1.0f, 0.0f, 0.0f };
        const auto globalUp      = glm::vec3{ 0.0f, 1.0f, 0.0f };
        const auto globalForward = glm::vec3{ 0.0f, 0.0f, 1.0f };

        const auto yaw   = glm::angleAxis(deltaRadians.x, globalUp);
        const auto pitch = glm::angleAxis(deltaRadians.y, globalRight);
        const auto roll  = glm::angleAxis(deltaRadians.z, globalForward);

        this->orientation =
            glm::normalize(yaw * pitch * roll * this->orientation);
    }

    auto getRotationMatrix() const
    {
        return glm::mat4_cast(this->orientation);
    }

private:
    glm::quat orientation;
};
} // namespace ctoAssetsRTIS
