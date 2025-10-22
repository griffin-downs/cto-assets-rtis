// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================

#pragma once

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "input/InputStates.h"

namespace ctoAssetsRTIS
{
class IdleInputSynthesizer
{
public:
    IdleInputSynthesizer() = default;

    auto synthesize(float deltaSeconds, const InputStates& userInput)
    {
        auto result = userInput; // start with real input

        const auto userActive =
            userInput.mouseStates.isContactActive() ||
            userInput.keyStates.anyKeyPressed();

        if (userActive)
        {
            this->idleTimerSeconds = 0.0f;
            this->timeSeconds = 0.0f;
            return result;
        }

        this->idleTimerSeconds += deltaSeconds;
        if (this->idleTimerSeconds < this->idleThresholdSeconds)
        {
            return result;
        }

        this->timeSeconds += deltaSeconds;

        const auto t = this->timeSeconds;
        const auto idleProgress =
            std::min(
                (this->idleTimerSeconds - this->idleThresholdSeconds) /
                this->rampDurationSeconds,
                1.0f);

        constexpr auto amplitude = 0.25f; // normalized delta units
        constexpr auto frequency = 0.15f; // Hz
        constexpr auto phaseOffset = glm::pi<float>() / 4.0f; // 45° phase shift

        // Create slow orbit-like motion
        const auto angle = 2.0f * glm::pi<float>() * frequency * t;
        const auto deltaX = amplitude * idleProgress * std::sin(angle + phaseOffset);
        const auto deltaY = amplitude * idleProgress * std::cos(angle);

        result.mouseStates.addDeviceMovement(deltaX, deltaY);
        return result;
    }

private:
    float idleTimerSeconds{ 0.0f };
    float timeSeconds{ 0.0f };

    float idleThresholdSeconds{ 3.0f }; // wait before starting idle drift
    float rampDurationSeconds{ 2.0f };  // smooth fade-in time
};
} // namespace ctoAssetsRTIS
