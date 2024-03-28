// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <chrono>
#include <exception>
#include <thread>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif


namespace ctoAssetsRTIS
{
template<typename T>
class FixedRateTimer
{
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<T, std::milli>;

public:
    struct FixedRateTimerConfiguration
    {
        T targetFPS;
    };
    FixedRateTimer(FixedRateTimerConfiguration configuration)
    : targetFrameDuration(
    [&]
    {
        const auto assertTargetFPS =
        [&]()
        {
            const auto targetFPS = configuration.targetFPS;

            if (targetFPS <= 0)
            {
                return false;
            }

            if (std::abs(targetFPS) < std::numeric_limits<T>::epsilon())
            {
                return false;
            }

            return true;
        };

        if (!assertTargetFPS())
        {
            throw std::invalid_argument("targetFPS must be greater than 0");
        }

        return Duration(T{1000.0} / configuration.targetFPS);
    }())
    , dt(this->targetFrameDuration.count())
    {
    }

    void startFrame()
    {
        this->frameStart = getTotalElapsedTimeMilliseconds();
    }

    void endFrame()
    {
        const auto frameEnd = getTotalElapsedTimeMilliseconds();
        const auto frameDuration = frameEnd - this->frameStart;

        if (frameDuration >= targetFrameDuration)
        {
            this->dt = frameDuration.count();
            return;
        }

        std::this_thread::sleep_for(targetFrameDuration - frameDuration);
        const auto sleepEnd = getTotalElapsedTimeMilliseconds();

        this->dt = (sleepEnd - this->frameStart).count();
    }

    T getDeltaTime() const
    {
        return this->dt;
    }

    static Duration getTotalElapsedTimeMilliseconds()
    {
#ifdef __EMSCRIPTEN__
        const auto getNowMs =
        []{ return static_cast<int64_t>(emscripten_get_now()); };
        static const auto initialTimepoint = getNowMs();
        return Duration(getNowMs() - initialTimepoint);
#else
        static const auto initialTimepoint = Clock::now();
        return
            std::chrono::duration_cast<
                Duration
            >(Clock::now() - initialTimepoint);
#endif
    }

private:
    Duration frameStart;
    Duration targetFrameDuration;
    T dt;
};
} // namespace ctoAssetsRTIS
