// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


namespace ctoAssetsRTIS
{
class Rotation
{
public:
    Rotation()
    : orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))
    {
    }

    struct EulerAnglesDegrees
    {
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;

        auto toQuaternion() const
        {
            return
                glm::quat(
                    glm::vec3(
                        glm::radians(pitch),
                        glm::radians(yaw),
                        glm::radians(roll)));
        }
    };
    Rotation(EulerAnglesDegrees angles)
    : orientation(angles.toQuaternion())
    {
    }

    void setEulerDegrees(EulerAnglesDegrees angles)
    {
        this->orientation = angles.toQuaternion();
    }

    void rotateEulerDegrees(EulerAnglesDegrees angles)
    {
        this->orientation = this->orientation * angles.toQuaternion();
    }

    auto getRotationMatrix() const
    {
        return glm::mat4_cast(this->orientation);
    }

private:
    glm::quat orientation;
};
} // namespace ctoAssetsRTIS