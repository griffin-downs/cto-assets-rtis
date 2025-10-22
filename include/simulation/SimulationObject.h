// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <glm/glm.hpp>

#include "AngularMotionController.h"
#include "graphics/model/Model.h"
#include "input/InputStates.h"
#include "Rotation.h"
#include "Transform.h"


namespace ctoAssetsRTIS
{
struct SimulationObject
{
    enum class Name
    {
        DodecahedronCage,
        VoronoiSphere
    };

    Name name;
    Model model;
    Transform transform;
    AngularMotionController angularMotionController;

    void applyInput(const InputStates& inputStates)
    {
        this->angularMotionController.applyInput(inputStates);
    }

    void updateMotion(float deltaSeconds)
    {
        const auto deltaRadians =
            this->angularMotionController.updateMotion(deltaSeconds);

        this->transform
            .rotation
            .applyDeltaRadians(deltaRadians);
    }
};
} // namespace ctoAssetsRTIS
