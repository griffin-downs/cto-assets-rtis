// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>
#include "AngularMotion.h"
#include "Rotation.h"
#include "Transform.h"
#include "input/InputStates.h"


namespace ctoAssetsRTIS
{
struct SimulationObject
{
    const Model& model;
    Transform transform;
    AngularMotion angularMotion;

    void update(
        const InputStates& inputStates,
        float dt)
    {
        angularMotion.addMouseImpulse(inputStates.mouseStates);
        angularMotion.addKeyboardImpulse(inputStates.keyStates);

        auto deltaRadians = angularMotion.updateMotion(dt);

        transform
            .rotation
            .rotateYawPitchRollRadians({
                .yaw = deltaRadians.x,
                .pitch = deltaRadians.y,
                .roll = deltaRadians.z
            });
    }
};
} // namespace ctoAssetsRTIS