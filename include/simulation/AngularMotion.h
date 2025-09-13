// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================

#pragma once

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Rotation.h"
#include "Transform.h"
#include "input/InputStates.h"

namespace ctoAssetsRTIS
{
class AngularMotion
{
public:
    struct Configuration
    {
        float mouseAngularVelocityGainRadiansPerUnit;
        float keyAngularAccelerationRadiansPerSecondSquared;
        float dampingRatePerSecond;
        float maxRadiansPerSecond = 12.566f;
    };

    AngularMotion()
    : AngularMotion(Configuration{})
    {
    }

    AngularMotion(Configuration c)
    : mouseAngularVelocityGainRadiansPerUnit{
        c.mouseAngularVelocityGainRadiansPerUnit
    }
    , keyAngularAccelerationRadiansPerSecondSquared{
        c.keyAngularAccelerationRadiansPerSecondSquared
    }
    , dampingRatePerSecond{ std::clamp(c.dampingRatePerSecond, 0.0001f, 1.0f) }
    , maxRadiansPerSecond{ std::max(0.0f, c.maxRadiansPerSecond) }
    {
    }

    template <typename VectorType>
    static constexpr auto isZeroVector(
        const VectorType& vector,
        typename VectorType::value_type epsilon =
            std::numeric_limits<typename VectorType::value_type>::epsilon()
    )
    {
        return glm::all(glm::epsilonEqual(vector, VectorType(0), epsilon));
    }

    void addMouseImpulse(const MouseStates& mouseStates)
    {
        if (!mouseStates.isContactActive())
        {
            return;
        }

        const auto delta = mouseStates.getPositionDeltaNormalized();

        const auto yawDeltaPerSec =
            -delta.x * this->mouseAngularVelocityGainRadiansPerUnit;

        const auto pitchDeltaPerSec =
            -delta.y * this->mouseAngularVelocityGainRadiansPerUnit;

        this->angularVelocityRadians.x += yawDeltaPerSec;
        this->angularVelocityRadians.y += pitchDeltaPerSec;
    }

    void addKeyboardImpulse(const KeyStates& keyStates)
    {
        auto k = this->keyAngularAccelerationRadiansPerSecondSquared;

        if (keyStates.aPressed)
        {
            this->angularVelocityRadians.x += -k;
        }
        if (keyStates.dPressed)
        {
            this->angularVelocityRadians.x += +k;
        }

        if (keyStates.wPressed)
        {
            this->angularVelocityRadians.y += -k;
        }
        if (keyStates.sPressed)
        {
            this->angularVelocityRadians.y += +k;
        }

        if (keyStates.qPressed)
        {
            this->angularVelocityRadians.z += -k;
        }
        if (keyStates.ePressed)
        {
            this->angularVelocityRadians.z += +k;
        }
    }

    auto getAngularSpeed() const
    {
        return glm::length(this->angularVelocityRadians);
    }

    auto getMaxAngularSpeed() const
    {
        return this->maxRadiansPerSecond;
    }

    auto updateMotion(float dt)
    {
        auto speed = this->getAngularSpeed();
        if (speed > this->maxRadiansPerSecond && speed > 0.0f)
        {
            this->angularVelocityRadians =
                (this->angularVelocityRadians / speed)
                    * this->maxRadiansPerSecond;
        }

        auto delta = this->angularVelocityRadians * dt;

        auto factor = std::max(0.0f, 1.0f - this->dampingRatePerSecond * dt);
        this->angularVelocityRadians *= factor;

        return delta;
    }

private:
    glm::vec3 angularVelocityRadians{ 0.0f, 0.0f, 0.0f };

    float mouseAngularVelocityGainRadiansPerUnit;
    float keyAngularAccelerationRadiansPerSecondSquared;
    float dampingRatePerSecond;
    float maxRadiansPerSecond;
};
} // namespace ctoAssetsRTIS
