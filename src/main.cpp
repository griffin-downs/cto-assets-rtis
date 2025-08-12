// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#include <functional>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "window/WindowSystem.h"

#include "OuterWireDodecahedron.cto.mtl.h"
#include "OuterWireDodecahedron.cto.obj.h"
#include "VoronoiSphere.cto.mtl.h"
#include "VoronoiSphere.cto.obj.h"

#include "graphics/camera/Camera.h"
#include "graphics/model/mesh/Mesh.h"
#include "graphics/model/Model.h"
#include "graphics/rendering/Renderer.h"
#include "graphics/rendering/ProjectionMatrixManager.h"
#include "input/InputSystem.h"
#include "simulation/FixedRateTimer.h"
#include "simulation/SimulationObject.h"


namespace ctoAssetsRTIS
{
class MainLoop
{
public:
    struct Configuration
    {
        std::function<bool()> nativeLoopPredicate;
    };
#ifndef __EMSCRIPTEN__
    MainLoop(Configuration configuration)
    : nativeLoopPredicate(configuration.nativeLoopPredicate)
    {
    }
#else
    MainLoop(Configuration)
    {
    }
#endif

    struct Parameters
    {
        std::function<void()> loopLogic;
    };
    void operator()(Parameters arguments)
    {
        auto& logic = arguments.loopLogic;

#ifndef __EMSCRIPTEN__
        while (this->nativeLoopPredicate())
        {
            logic();
        }
#else
        auto wrappedLogic =
            std::function<void()>
            {
                [&]
                {
                    try
                    {
                        logic();
                    }
                    catch (const std::exception& exception)
                    {
                        emscripten_cancel_main_loop();
                        throw exception;
                    }
                }
            };

        emscripten_set_main_loop_arg(
            [](void* functionPointer)
            {
                auto& function =
                    *reinterpret_cast<
                        std::function<void()>*
                    >(functionPointer);

                function();
            },
            &wrappedLogic,
            0,
            1);
#endif
    }
private:
#ifndef __EMSCRIPTEN__
    std::function<bool()> nativeLoopPredicate;
#endif
};
} // namespace ctoAssetsRTIS

int main()
{
    using namespace ctoAssetsRTIS;

    std::cout << "Hello!\n";

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

        auto camera = Camera({
            .radius = 2.0f,
            .rotationSensitivity = 0.0001f,
            .dampingFactor = 0.0005f,
            .initialYawDegrees = 35.0f,
            .initialPitchDegrees = -20.0f
        });

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

        const auto simulationObjects =
            std::to_array({
                SimulationObject
                {
                    .model = outerWireDodecahedronModel,
                    .transform = Transform({ .scale = glm::vec3(0.04f) })
                },
                SimulationObject
                {
                    .model = voronoiSphereModel,
                    .transform = Transform({ .scale = glm::vec3(0.035f) })
                }
            });

        auto renderer = Renderer({
            .camera = camera,
            .projectionMatrixManager = projectionMatrixManager
        });

        auto timer = FixedRateTimer<float>({ .targetFPS = 60.0f });

        MainLoop({
            .nativeLoopPredicate = [&]{ return !inputSystem.isExitRequested(); }
        })({
            .loopLogic =
            [&]
            {
                timer.startFrame();

                windowSystem.clearScreen();

                camera.update({
                    .inputStates = inputSystem.getStates(),
                    .viewportDimensions =
                        projectionMatrixManager.getViewportDimensions(),
                    .dt = timer.getDeltaTime()
                });

                renderer.render({ simulationObjects });

                windowSystem.swapBuffers();

                inputSystem.pollEvents();

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
