// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Input/InputStates.h"


namespace ctoAssetsRTIS
{
class Camera
{
private:
    static auto getGlobalRightVector()
    {
        static const auto rightVector = glm::vec3(1.0f, 0.0f, 0.0f);
        return rightVector;
    }

    static auto getGlobalUpVector()
    {
        static const auto upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        return upVector;
    }

    static auto getGlobalForwardVector()
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
                getGlobalUpVector());

        const auto pitchQuaternion =
            glm::angleAxis(
                glm::radians(configuration.initialPitchDegrees),
                getGlobalRightVector());

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
    auto getCameraForwardVector() const
    {
        return this->orientation * -getGlobalForwardVector();
    }

    template<typename VectorType>
    static auto isZeroVector(const VectorType& vector)
    {
        const auto allEpsilonEqual =
        [](
            const auto& vector1,
            const auto& vector2,
            float epsilon = std::numeric_limits<float>::epsilon())
        {
            return glm::all(glm::epsilonEqual(vector1, vector2, epsilon));
        };

        static auto zeroVector = VectorType();
        return allEpsilonEqual(vector, zeroVector);
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
        this->processMouseInput(
            inputStates.mouseStates,
            arguments.viewportDimensions,
            dt);
        this->processKeyInput(inputStates.keyStates, dt);

        if (!isZeroVector(this->velocity))
        {
            const auto yawQuaternion =
                glm::angleAxis(
                    this->velocity.x,
                    this->orientation * getGlobalUpVector());

            const auto pitchQuaternion =
                glm::angleAxis(
                    this->velocity.y,
                    this->orientation * getGlobalRightVector());

            const auto rollQuaternion =
                glm::angleAxis(
                    this->velocity.z,
                    this->orientation * getGlobalForwardVector());

            this->orientation = yawQuaternion * this->orientation;
            this->orientation = pitchQuaternion * this->orientation;
            this->orientation = rollQuaternion * this->orientation;

            this->orientation = glm::normalize(this->orientation);
        }

        this->position = -this->getCameraForwardVector() * radius;
        if (glm::length(this->position) != radius)
        {
            this->position = glm::normalize(this->position) * radius;
        }

        this->velocity *= 1.0f - std::clamp(dampingFactor * dt, 0.0f, 1.0f);
    }

    void processMouseInput(
        const MouseStates& mouseStates,
        ViewportDimensions viewportDimensions,
        float dt)
    {
        if (!mouseStates.buttonPressed && !mouseStates.touchActive)
        {
            return;
        }

        const auto cursorDidNotMove =
        [&]
        {
            static auto previousCurrentCursorPosition = glm::vec2();

            const auto cursorPositionDelta =
                mouseStates.currentCursorPosition
                    - previousCurrentCursorPosition;

            previousCurrentCursorPosition = mouseStates.currentCursorPosition;


            return isZeroVector(cursorPositionDelta);
        }();

        if (cursorDidNotMove)
        {
            return;
        }

        const auto clampToViewportDimensions =
        [&](const auto& vector)
        {
            return
                glm::clamp(
                    vector,
                    glm::vec2(),
                    glm::vec2(
                        viewportDimensions.getWidth(),
                        viewportDimensions.getHeight()));
        };

        const auto clampedLastPosition =
            clampToViewportDimensions(mouseStates.lastCursorPosition);

        const auto clampedCurrentPosition =
            clampToViewportDimensions(mouseStates.currentCursorPosition);

        auto deltaVelocity =
            glm::vec3(clampedLastPosition - clampedCurrentPosition, 0.0f);

        if (isZeroVector(deltaVelocity))
        {
            return;
        }

        deltaVelocity.x /= viewportDimensions.getWidth();
        deltaVelocity.y /= viewportDimensions.getHeight();

        deltaVelocity =
            glm::normalize(deltaVelocity)
                * std::clamp(
                    glm::length(deltaVelocity),
                    0.0f,
                    glm::radians(2.5f));

        const auto inputBasedAccelerationFactor =
            mouseStates.touchActive
                ? 150.0f
                : 100.0f;

        deltaVelocity *=
            inputBasedAccelerationFactor
                * this->rotationSensitivity
                * dt;

        this->velocity += deltaVelocity;
    }

    void processKeyInput(const KeyStates& keyStates, float dt)
    {
        auto deltaVelocity = glm::vec3();
        if (keyStates.wPressed)
        {
            deltaVelocity.y += 1.0f;
        }
        if (keyStates.aPressed)
        {
            deltaVelocity.x -= 1.0f;
        }
        if (keyStates.sPressed)
        {
            deltaVelocity.y -= 1.0f;
        }
        if (keyStates.dPressed)
        {
            deltaVelocity.x += 1.0f;
        }
        if (keyStates.qPressed)
        {
            deltaVelocity.z -= 1.0f;
        }
        if (keyStates.ePressed)
        {
            deltaVelocity.z += 1.0f;
        }

        if (isZeroVector(deltaVelocity))
        {
            return;
        }

        deltaVelocity =
            glm::normalize(deltaVelocity)
                * this->rotationSensitivity
                * dt;

        this->velocity += deltaVelocity;
    }

    auto getViewMatrix() const
    {
        return glm::lookAt(
            this->position,
            this->position + this->getCameraForwardVector(),
            this->orientation * getGlobalUpVector());
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