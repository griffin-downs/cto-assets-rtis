// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#include <functional>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "Window/WindowSystem.h"

#include "RubiksCubeMtl.h"
#include "RubiksCubeObj.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Model/Mesh/Mesh.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Rendering/Renderer.h"
#include "Graphics/Rendering/ProjectionMatrixManager.h"
#include "Input/InputSystem.h"
#include "Simulation/FixedRateTimer.h"
#include "Simulation/SimulationObject.h"


namespace ctoAssetsRTIS
{
class MainLoop
{
public:
    template<auto NativeLoopPredicate>
    struct Configuration
    {
        static constexpr auto nativeLoopPredicate = NativeLoopPredicate;
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
        auto& logic = arguments.logic;

#ifndef __EMSCRIPTEN__
        while (!inputSystem.isExitRequested())
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
            .rotationSensitivity = 0.0002f,
            .dampingFactor = 0.002f,
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

        const auto rubiksCubeModel =
            Model
            {
                .mesh = deserialize<Mesh, fileContents::RubiksCubeObj>(),
                .materialLibrary =
                    deserialize<MaterialLibrary, fileContents::RubiksCubeMtl>()
            };

        const auto simulationObjects =
            std::to_array({
                SimulationObject
                {
                    .model = rubiksCubeModel,
                    .transform = Transform({ .scale = glm::vec3(0.075f) })
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
