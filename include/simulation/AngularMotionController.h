// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include "input/InputStates.h"

namespace ctoAssetsRTIS
{
class AngularMotionController
{
public:
    struct Configuration
    {
        float mouseGainRadiansPerUnit;
        float keyGainRadiansPerSecondSquared;
        float dampingRatePerSecond;
    };
    AngularMotionController(Configuration configuration)
    : mouseGainRadiansPerUnit{ configuration.mouseGainRadiansPerUnit }
    , keyGainRadiansPerSecondSquared
    {
        configuration.keyGainRadiansPerSecondSquared
    }
    , dampingRatePerSecond
    {
        std::clamp(configuration.dampingRatePerSecond, 0.0001f, 1.0f)
    }
    {}

    auto applyInput(const InputStates& inputStates)
    {
        const auto& mouse = inputStates.mouseStates;
        const auto& keys  = inputStates.keyStates;

        if (mouse.isContactActive())
        {
            const auto delta = mouse.getPositionDeltaNormalized();
            this->impulse +=
                glm::vec3
                {
                    -delta.x * this->mouseGainRadiansPerUnit,
                    -delta.y * this->mouseGainRadiansPerUnit,
                    0.0f
                };
        }

        const auto gain = this->keyGainRadiansPerSecondSquared;

        if (keys.aPressed || keys.leftPressed)
            this->angularAcceleration.x -= gain;
        if (keys.dPressed || keys.rightPressed)
            this->angularAcceleration.x += gain;
        if (keys.wPressed || keys.upPressed)
            this->angularAcceleration.y -= gain;
        if (keys.sPressed || keys.downPressed)
            this->angularAcceleration.y += gain;
        if (keys.qPressed)
            this->angularAcceleration.z -= gain;
        if (keys.ePressed)
            this->angularAcceleration.z += gain;
    }

    auto updateMotion(float stepSeconds)
    {
        const auto decay = std::exp(-this->dampingRatePerSecond * stepSeconds);
        this->angularVelocityRadians *= decay;

        this->angularVelocityRadians +=
            this->angularAcceleration * stepSeconds + this->impulse;

        this->angularAcceleration = glm::vec3{};
        this->impulse             = glm::vec3{};

        return this->angularVelocityRadians;
    }

    auto getAngularSpeedDegreesPerSecond() const
    {
        return glm::degrees(glm::length(this->angularVelocityRadians));
    }

private:
    glm::vec3 angularVelocityRadians{ 0.0f };
    glm::vec3 angularAcceleration{ 0.0f };
    glm::vec3 impulse{ 0.0f };

    float mouseGainRadiansPerUnit;
    float keyGainRadiansPerSecondSquared;
    float dampingRatePerSecond;
};
} // namespace ctoAssetsRTIS
