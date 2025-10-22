// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "window/ViewportDimensions.h"
#include "MovementAccumulator.h"


namespace ctoAssetsRTIS
{
struct KeyStates
{
    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;
    bool qPressed = false;
    bool ePressed = false;

    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    void setKeyState(int key, bool pressed)
    {
        switch (key)
        {
            case GLFW_KEY_W:
                wPressed = pressed;
                break;
            case GLFW_KEY_A:
                aPressed = pressed;
                break;
            case GLFW_KEY_S:
                sPressed = pressed;
                break;
            case GLFW_KEY_D:
                dPressed = pressed;
                break;
            case GLFW_KEY_Q:
                qPressed = pressed;
                break;
            case GLFW_KEY_E:
                ePressed = pressed;
                break;
            case GLFW_KEY_UP:
                upPressed = pressed;
                break;
            case GLFW_KEY_DOWN:
                downPressed = pressed;
                break;
            case GLFW_KEY_LEFT:
                leftPressed = pressed;
                break;
            case GLFW_KEY_RIGHT:
                rightPressed = pressed;
                break;
            default:
                break;
        }
    }

    auto anyKeyPressed() const
    {
        return
            this->wPressed    || this->aPressed     || this->sPressed ||
            this->dPressed    || this->qPressed     || this->ePressed ||
            this->upPressed   || this->downPressed  ||
            this->leftPressed || this->rightPressed;
    }
};

class MouseStates
{
private:
    static auto clampToViewport(int value, int maximum)
    {
        return
            std::clamp(
                value,
                0,
                maximum - 1);
    }

    static auto clampToViewport(double value, int maximum)
    {
        auto rounded = static_cast<int>(std::lround(value));
        return clampToViewport(rounded, maximum);
    }

public:
    MouseStates() = default;

    void startFrame()
    {
        this->movementAccumulator.startFrame();
    }

    auto getPositionDeltaNormalized() const
    {
        return
            this->movementAccumulator
                .getDeltaNormalized(this->viewportDimensions);
    }

    void updateDevicePosition(
        double deviceX,
        double deviceY,
        ViewportDimensions viewportDimensions)
    {
        this->viewportDimensions = viewportDimensions;

        const auto clamp = [&](double value, int maxValue)
        {
            auto i = static_cast<int>(std::lround(value));
            return std::clamp(i, 0, maxValue - 1);
        };

        const auto clampedX =
            clamp(deviceX, this->viewportDimensions.getWidth());
        const auto clampedY =
            clamp(deviceY, this->viewportDimensions.getHeight());

        this->movementAccumulator.addSamplePixels(
            static_cast<float>(clampedX),
            static_cast<float>(clampedY));
    }

    void addDeviceMovement(double deltaX, double deltaY)
    {
        this->updateDevicePosition(
            deltaX * viewportDimensions.getWidth(),
            deltaY * viewportDimensions.getHeight(),
            this->viewportDimensions);
    }

    auto isContactActive() const
    {
        return this->contactActive;
    }

    void beginContact()
    {
        this->contactActive = true;
        this->movementAccumulator.resetBaseline();
    }

    void endContact()
    {
        this->contactActive = false;
    }

private:
    MovementAccumulator movementAccumulator{};
    ViewportDimensions viewportDimensions{{ .width = 1, .height = 1 }};
    bool contactActive = false;
};

struct InputStates
{
    MouseStates mouseStates;
    KeyStates keyStates;
};
} // namespace ctoAssetsRTIS
