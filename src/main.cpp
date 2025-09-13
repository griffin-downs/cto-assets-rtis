// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#include <iostream>

#include "window/WindowSystem.h"

#include "OuterWireDodecahedron.cto.mtl.h"
#include "OuterWireDodecahedron.cto.obj.h"
#include "VoronoiSphere.cto.mtl.h"
#include "VoronoiSphere.cto.obj.h"

#include "graphics/model/mesh/Mesh.h"
#include "graphics/model/Model.h"
#include "graphics/rendering/Renderer.h"
#include "graphics/rendering/ProjectionMatrixManager.h"
#include "input/InputSystem.h"
#include "simulation/FixedRateTimer.h"
#include "simulation/SimulationObject.h"
#include "simulation/GameLoop.h"


int main()
{
    using namespace ctoAssetsRTIS;

    try
    {
        auto projectionMatrixManager = ProjectionMatrixManager();

        const auto windowSystem =
            WindowSystem({
                .title = "cto-assets-rtis",
                .viewportDimensions =
                    projectionMatrixManager.getViewportDimensions()
            });

        auto inputSystem =
            InputSystem({ .window = windowSystem.getWindow() });

        auto applicationStateManager =
            ApplicationStateManager({
                .projectionMatrixManager = projectionMatrixManager,
                .inputSystem = inputSystem
            });

        applicationStateManager.setWindowUserDataToHeldState({
            .window = windowSystem.getWindow().get()
        });

        const auto outerWireDodecahedronModel =
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
            };

        const auto voronoiSphereModel =
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
            };

        auto simulationObjects =
            std::to_array({
                SimulationObject
                {
                    .model = outerWireDodecahedronModel,
                    .transform = Transform({ .scale = glm::vec3(0.4f) }),
                    .angularMotion =
                        AngularMotion({
                            .mouseAngularVelocityGainRadiansPerUnit =
                                0.02f,
                            .keyAngularAccelerationRadiansPerSecondSquared =
                                .0002f,
                            .dampingRatePerSecond = 0.0015f
                        })
                },
                SimulationObject
                {
                    .model = voronoiSphereModel,
                    .transform = Transform({ .scale = glm::vec3(0.35f) }),
                    .angularMotion =
                        AngularMotion({
                            .mouseAngularVelocityGainRadiansPerUnit =
                                0.015f,
                            .keyAngularAccelerationRadiansPerSecondSquared =
                                .00015f,
                            .dampingRatePerSecond = 0.0005f
                        })
                }
            });

        auto renderer = Renderer({
            .projectionMatrixManager = projectionMatrixManager
        });

        auto timer = FixedRateTimer<float>({ .targetFPS = 60.0f });

        GameLoop({
            .nativeLoopPredicate = [&]{ return !inputSystem.isExitRequested(); }
        })({
            .loopLogic =
            [&]
            {
                timer.startFrame();

                windowSystem.clearScreen();

                inputSystem.startFrame();

                for (auto& object : simulationObjects)
                {
                    object.update(
                        inputSystem.getStates(),
                        timer.getDeltaTime());
                }

                renderer.render({ simulationObjects });

                windowSystem.swapBuffers();

                timer.endFrame();
            }
        });
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
        return -1;
    }
}
