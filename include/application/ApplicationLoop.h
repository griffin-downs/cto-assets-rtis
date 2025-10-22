// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <functional>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif


namespace ctoAssetsRTIS
{
class ApplicationLoop
{
public:
    struct Configuration
    {
        std::function<bool()> nativeLoopPredicate;
    };
#ifndef __EMSCRIPTEN__
    ApplicationLoop(Configuration configuration)
    : nativeLoopPredicate{ configuration.nativeLoopPredicate }
    {}
#else
    ApplicationLoop(Configuration)
    {}
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
            0 /* fps */,
            1 /* simulate_infinite_loop */);
#endif
    }
private:
#ifndef __EMSCRIPTEN__
    std::function<bool()> nativeLoopPredicate;
#endif
};
} // namespace ctoAssetsRTIS
