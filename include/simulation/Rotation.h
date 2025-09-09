// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
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

    struct EulerAnglesRadians
    {
        float yaw = 0.0f;
        float pitch = 0.0f;
        float roll = 0.0f;

        auto toQuaternion() const
        {
            return
                glm::quat(
                    glm::vec3(
                        pitch,
                        yaw,
                        roll));
        }
    };
    Rotation(EulerAnglesRadians angles)
    : orientation(angles.toQuaternion())
    {
    }

    void rotateYawPitchRollRadians(EulerAnglesRadians eulerAnglesRadians)
    {
        const auto [
            yaw,
            pitch,
            roll
        ] = eulerAnglesRadians;

        const auto up      = glm::vec3(0.0f, 1.0f, 0.0f);
        const auto right   = glm::vec3(1.0f, 0.0f, 0.0f);
        const auto forward = glm::vec3(0.0f, 0.0f, 1.0f);

        auto qYaw =
            glm::angleAxis(
                yaw,
                glm::vec3(0.0f, 1.0f, 0.0f));

        auto qPitch =
            glm::angleAxis(
                pitch,
                glm::vec3(1.0f, 0.0f, 0.0f));

        auto qRoll =
            glm::angleAxis(
                roll,
                glm::vec3(0.0f, 0.0f, 1.0f));

        this->orientation = qYaw * this->orientation;
        this->orientation = qPitch * this->orientation;
        this->orientation = qRoll * this->orientation;

        this->orientation = glm::normalize(this->orientation);
    }

    auto getRotationMatrix() const
    {
        return glm::mat4_cast(this->orientation);
    }

private:
    glm::quat orientation;
};
} // namespace ctoAssetsRTIS