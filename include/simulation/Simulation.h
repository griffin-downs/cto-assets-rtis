// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>

#include "AngularMotionController.h"
#include "graphics/model/Model.h"
#include "graphics/model/mesh/Mesh.h"
#include "OuterWireDodecahedron.cto.mtl.h"
#include "OuterWireDodecahedron.cto.obj.h"
#include "physics/FixedTimeStepper.h"
#include "serialization/DeserializeMesh.h"
#include "serialization/DeserializeMaterialLibrary.h"
#include "SimulationObject.h"
#include "VoronoiSphere.cto.mtl.h"
#include "VoronoiSphere.cto.obj.h"
#include "input/IdleInputSynthesizer.h"

namespace ctoAssetsRTIS
{
namespace
{
static auto makeSimulationObjects()
{
    return
        std::array
        {
            SimulationObject
            {
                .name = SimulationObject::Name::DodecahedronCage,
                .model =
                    Model
                    {
                        .mesh =
                            deserialize<
                                Mesh,
                                fileContents::OuterWireDodecahedronCtoObj
                            >(),
                        .materialLibrary =
                            deserialize<
                                MaterialLibrary,
                                fileContents::OuterWireDodecahedronCtoMtl
                            >()
                    },
                .transform = Transform({ .scale = glm::vec3(0.4f) }),
                .angularMotionController =
                    AngularMotionController({
                        .mouseGainRadiansPerUnit = 0.06f,
                        .keyGainRadiansPerSecondSquared = 0.03f,
                        .dampingRatePerSecond = 0.6f
                    })
            },
            SimulationObject
            {
                .name = SimulationObject::Name::VoronoiSphere,
                .model =
                    Model
                    {
                        .mesh =
                            deserialize<
                                Mesh,
                                fileContents::VoronoiSphereCtoObj
                            >(),
                        .materialLibrary =
                            deserialize<
                                MaterialLibrary,
                                fileContents::VoronoiSphereCtoMtl
                            >()
                    },
                .transform = Transform({ .scale = glm::vec3(0.35f) }),
                .angularMotionController =
                    AngularMotionController({
                        .mouseGainRadiansPerUnit = 0.03f,
                        .keyGainRadiansPerSecondSquared = 0.015f,
                        .dampingRatePerSecond = 0.3f
                    })
            }
        };
}
} // unnamed namespace

class Simulation
{
public:
    void update(float frameDeltaSeconds, const InputStates& inputStates)
    {
        const auto synthesizedInput =
            this->idleInputSynthesizer.synthesize(frameDeltaSeconds, inputStates);

        this->fixedTimeStepper.addSeconds(frameDeltaSeconds);
        auto fixedStepRange = this->fixedTimeStepper.consumeSteps();

        for (auto& simulationObject : this->simulationObjects)
        {
            simulationObject.applyInput(synthesizedInput);
        }

        const auto updateMotion =
        [&](float stepSeconds)
        {
            for (auto& simulationObject : this->simulationObjects)
            {
                simulationObject.updateMotion(stepSeconds);
            }
        };

        for (const auto stepSeconds : fixedStepRange)
        {
            updateMotion(stepSeconds);
        }
    }

    auto getObjects() const
    {
        return std::span{ std::as_const(this->simulationObjects) };
    }

private:
    decltype(makeSimulationObjects()) simulationObjects
    {
        makeSimulationObjects()
    };

    FixedTimeStepper fixedTimeStepper;
    IdleInputSynthesizer idleInputSynthesizer;
};
} // namespace ctoAssetsRTIS
