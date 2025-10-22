// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <algorithm>
#include <ranges>


namespace ctoAssetsRTIS
{
struct FixedStepRange
{
    int steps;
    float stepSeconds;

    struct Iterator
    {
        int index{};
        int count{};
        float stepSeconds{};

        auto operator*() const { return stepSeconds; }

        auto operator++() { ++index; return *this; }

        auto operator!=(const Iterator& other) const
        {
            return index != other.index;
        }
    };

    auto begin() const { return Iterator{0, steps, stepSeconds}; }
    auto end()   const { return Iterator{steps, steps, stepSeconds}; }
};

class FixedTimeStepper
{
public:
    auto addSeconds(float deltaSeconds)
    {
        constexpr auto maxDeltaSeconds = 0.25f;
        if (deltaSeconds > maxDeltaSeconds)
        {
            return;
        }

        constexpr auto smoothingFactor = 0.1f;
        this->smoothedDeltaSeconds =
            smoothingFactor * deltaSeconds
                + (1.0f - smoothingFactor) * smoothedDeltaSeconds;

        this->accumulatedSeconds += smoothedDeltaSeconds;
    }

    auto consumeSteps()
    {
        constexpr auto stepSeconds = 1.0f / 120.0f;
        constexpr auto maxSteps = 8;

        const auto steps = std::min(
            maxSteps,
            static_cast<int>(this->accumulatedSeconds / stepSeconds));

        this->accumulatedSeconds -= steps * stepSeconds;

        return FixedStepRange{ steps, stepSeconds };
    }

private:
    float accumulatedSeconds{ 0.0f };
    float smoothedDeltaSeconds{ 0.0f };
};
} // namespace ctoAssetsRTIS
