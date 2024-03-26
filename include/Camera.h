// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "InputStates.h"


namespace ctoAssetsRTIS
{
class Camera
{
private:
    static auto getRightVector()
    {
        static const auto rightVector = glm::vec3(1.0f, 0.0f, 0.0f);
        return rightVector;
    }

    static auto getUpVector()
    {
        static const auto upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        return upVector;
    }

    static auto getForwardVector()
    {
        static const auto forwardVector = glm::vec3(0.0f, 0.0f, 1.0f);
        return forwardVector;
    }
public:
    struct CameraConfiguration
    {
        float radius;
        float rotationSensitivity;
        float dampingFactor;
        float initialYawDegrees;
        float initialPitchDegrees;
    };
    Camera(CameraConfiguration configuration)
    : orientation(
    [&]
    {
        const auto yawQuaternion =
            glm::angleAxis(
                glm::radians(configuration.initialYawDegrees),
                getUpVector());

        const auto pitchQuaternion =
            glm::angleAxis(
                glm::radians(configuration.initialPitchDegrees),
                getRightVector());

        return pitchQuaternion * yawQuaternion;
    }())
    , position(0.0f, 0.0f, configuration.radius)
    , velocity(0.0f, 0.0f, 0.0f)
    , radius{ configuration.radius }
    , rotationSensitivity{ configuration.rotationSensitivity }
    , dampingFactor{ configuration.dampingFactor }
    {
    }

    Camera(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(const Camera&) = delete;

private:
    auto calculateFrontVector() const
    {
        return this->orientation * glm::vec3(0.0f, 0.0f, -1.0f);
    }

public:
    struct UpdateArguments
    {
        const InputStates& inputStates;
        ViewportDimensions viewportDimensions;
        float dt;
    };
    void update(UpdateArguments arguments)
    {
        const auto dt = arguments.dt;

        const auto& inputStates = arguments.inputStates;
        this->processInput(
            inputStates.mouseStates,
            arguments.viewportDimensions,
            dt);
        this->processInput(inputStates.keyStates, dt);

        const float angleMagnitude = glm::length(this->velocity) * dt;
        if (angleMagnitude >= std::numeric_limits<float>::epsilon())
        {
            const auto yawQuaternion =
                glm::angleAxis(
                    this->velocity.x,
                    this->orientation * getUpVector());

            const auto pitchQuaternion =
                glm::angleAxis(
                    this->velocity.y,
                    this->orientation * getRightVector());

            const auto rollQuaternion =
                glm::angleAxis(
                    this->velocity.z,
                    this->orientation * getForwardVector());

            this->orientation = yawQuaternion * this->orientation;
            this->orientation = pitchQuaternion * this->orientation;
            this->orientation = rollQuaternion * this->orientation;

            this->orientation = glm::normalize(this->orientation);
        }

        this->position = -this->calculateFrontVector() * radius;
        if (glm::length(this->position) != radius)
        {
            this->position = glm::normalize(this->position) * radius;
        }

        this->velocity *= 1.0f - std::clamp(dampingFactor * dt, 0.0f, 1.0f);
    }

    void processInput(
        const MouseStates& mouseStates,
        ViewportDimensions viewportDimensions,
        float dt)
    {
        if (!mouseStates.buttonPressed && !mouseStates.touchActive)
        {
            return;
        }

        const auto clampToViewportDimensions =
        [&](const auto& vec)
        {
            return
                glm::clamp(
                    vec,
                    glm::vec2(),
                    glm::vec2(
                        viewportDimensions.getWidth(),
                        viewportDimensions.getHeight()));
        };

        const auto clampedLastPosition =
            clampToViewportDimensions(mouseStates.lastCursorPosition);

        const auto clampedCurrentPosition =
            clampToViewportDimensions(mouseStates.currentCursorPosition);

        auto touchMovement =
            glm::vec3(clampedLastPosition - clampedCurrentPosition, 0.0f);

        touchMovement.x /= viewportDimensions.getWidth();
        touchMovement.y /= viewportDimensions.getHeight();

        const auto movementMagnitude = glm::length(touchMovement);
        if (movementMagnitude <= std::numeric_limits<float>::epsilon())
        {
            return;
        }

        touchMovement =
            glm::normalize(touchMovement) *
                std::clamp(movementMagnitude, 0.0f, glm::radians(2.5f));

        const auto accelerationFactor =
            mouseStates.touchActive
                ? 150.0f
                : 100.0f;

        touchMovement *= this->rotationSensitivity * dt * accelerationFactor;
        this->velocity += touchMovement;
    }

    void processInput(const KeyStates& keyStates, float dt)
    {
        auto deltaVelocity = glm::vec3(0.0f);
        const auto acceleration = this->rotationSensitivity * dt;
        if (keyStates.wPressed)
        {
            deltaVelocity.y += acceleration;
        }
        if (keyStates.aPressed)
        {
            deltaVelocity.x -= acceleration;
        }
        if (keyStates.sPressed)
        {
            deltaVelocity.y -= acceleration;
        }
        if (keyStates.dPressed)
        {
            deltaVelocity.x += acceleration;
        }
        if (keyStates.qPressed)
        {
            deltaVelocity.z -= acceleration * 2.0f;
        }
        if (keyStates.ePressed)
        {
            deltaVelocity.z += acceleration * 2.0f;
        }

        this->velocity += deltaVelocity;
    }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(
            this->position,
            this->position + this->calculateFrontVector(),
            this->orientation * glm::vec3(0.0f, 1.0f, 0.0f));
    }

private:
    glm::quat orientation;
    glm::vec3 position;
    glm::vec3 velocity;

    float radius;
    float rotationSensitivity;
    float dampingFactor;
};
} // namespace ctoAssetsRTIS