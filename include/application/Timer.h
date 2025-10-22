// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace ctoAssetsRTIS
{
class Timer
{
    using Duration  = std::chrono::duration<float>;
    using TimePoint = std::chrono::duration<float, std::milli>;

private:
    static auto getNowMilliseconds()
    {
#ifdef __EMSCRIPTEN__
        const auto nowMs = static_cast<float>(emscripten_get_now());
        static const auto initialMs = nowMs;
        return TimePoint(nowMs - initialMs);
#else
        static const auto start = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<TimePoint>(
            std::chrono::high_resolution_clock::now() - start);
#endif
    }

public:
    Timer()
    : previous(getNowMilliseconds())
    {}

    void reset()
    {
        previous = getNowMilliseconds();
    }

    float getDeltaSeconds()
    {
        const auto now = getNowMilliseconds();
        const auto delta = std::chrono::duration_cast<Duration>(now - previous).count();
        previous = now;
        return delta;
    }

private:
    TimePoint previous;
};
} // namespace ctoAssetsRTIS
