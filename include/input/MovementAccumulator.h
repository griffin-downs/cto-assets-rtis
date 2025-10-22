// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/vec2.hpp>
#include <algorithm>
#include <cmath>

#include "window/ViewportDimensions.h"


namespace ctoAssetsRTIS
{
class MovementAccumulator
{
public:
    void startFrame()
    {
        this->frameDeltaPixels = this->pendingDeltaPixels;
        this->pendingDeltaPixels = glm::vec2{ 0.0f, 0.0f };
    }

    void addSamplePixels(float xPixels, float yPixels)
    {
        auto current = glm::vec2{ xPixels, yPixels };

        if (!this->haveBaseline)
        {
            this->lastSamplePixels = current;
            this->haveBaseline = true;
            return;
        }

        this->pendingDeltaPixels += (this->lastSamplePixels - current);
        this->lastSamplePixels = current;
    }

    void addPendingDeltaPixels(float deltaX, float deltaY)
    {
        this->frameDeltaPixels += glm::vec2 { deltaX, deltaY };
    }

    void resetBaseline()
    {
        this->haveBaseline = false;
        this->frameDeltaPixels = glm::vec2{ 0.0f, 0.0f };
    }

    auto getDeltaPixels() const
    {
        return this->frameDeltaPixels;
    }

    auto getDeltaNormalized(ViewportDimensions viewportDimensions) const
    {
        auto normalizedX =
            frameDeltaPixels.x
                / static_cast<float>(
                    std::max(1, viewportDimensions.getWidth()));
        auto normalizedY =
            frameDeltaPixels.y
                / static_cast<float>(
                    std::max(1, viewportDimensions.getHeight()));

        return glm::vec2{ normalizedX, normalizedY };
    }

private:
    glm::vec2 pendingDeltaPixels{ 0.0f, 0.0f };
    glm::vec2 frameDeltaPixels{ 0.0f, 0.0f };
    glm::vec2 lastSamplePixels{ 0.0f, 0.0f };
    bool      haveBaseline = false;
};
} // namespace ctoAssetsRTIS
