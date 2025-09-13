// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <algorithm>
#include <type_traits>


namespace ctoAssetsRTIS
{
template<typename T, T Min, T Max>
class Clamped
{
    static_assert(std::is_arithmetic<T>::value);

public:

    constexpr Clamped(T value)
    : value(std::clamp(value, Min, Max))
    {
    }

    constexpr auto get() const
    {
        return this->value;
    }

private:
    T value;
};

template<typename T, T Min>
struct AtLeast
{
    static_assert(std::is_arithmetic<T>::value);

public:
    constexpr AtLeast(T value)
    : value(value < Min ? Min : value)
    {
    }

    constexpr auto get() const
    {
        return this->value;
    }

private:
    T value;
};

using NormalizedFloat = Clamped<float, 0.0f, 1.0f>;
using AtLeast1Float = AtLeast<float, 1.0f>;
} // namespace ctoAssetsRTIS
