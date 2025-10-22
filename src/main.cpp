// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#include <iostream>
#include <cstdlib>

#include "window/Window.h"
#include "application/ApplicationLoop.h"
#include "application/Timer.h"
#include "graphics/rendering/Renderer.h"
#include "graphics/rendering/ProjectionMatrixManager.h"
#include "simulation/Simulation.h"


int main()
{
    using namespace ctoAssetsRTIS;

    try
    {
        auto projectionMatrixManager = ProjectionMatrixManager{};
        auto window = Window{ projectionMatrixManager };
        auto simulation = Simulation{};
        auto renderer = Renderer{ projectionMatrixManager };
        auto hostTimer = Timer{};

        ApplicationLoop({
            .nativeLoopPredicate = [&]{ return !window.isExitRequested(); }
        })({
            .loopLogic =
            [&]
            {
                const auto deltaSeconds = hostTimer.getDeltaSeconds();
                std::cout << deltaSeconds << std::endl;
                window.startFrame();
                simulation.update(deltaSeconds, window.getInputStates());
                renderer.render(simulation.getObjects());
                window.swapBuffers();
                hostTimer.reset();
            }
        });
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
}
