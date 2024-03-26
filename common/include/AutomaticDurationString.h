// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <algorithm>
#include <array>
#include <string_view>

namespace ctoAssetsRTIS
{
template<size_t N>
struct AutomaticDurationString
{
    char data[N];

    static constexpr auto length = N;

    constexpr AutomaticDurationString() {}

    constexpr AutomaticDurationString(const char (&data)[N])
    {
        std::copy_n(data, N, this->data);
    }

    explicit constexpr AutomaticDurationString(const std::array<char, N>& data)
    {
        std::copy_n(data.data(), N, this->data);
    }

    constexpr std::string_view toStringView() const
    {
        return std::string_view(data, N - 1);
    }
};

template<size_t... Ns>
constexpr auto makeAutomaticDurationString(const char (&...strings)[Ns]) {
    constexpr size_t totalSize = (... + (sizeof(strings) - 1)) + 1;
    auto buffer = std::array<char, totalSize>{};

    auto offset = buffer.begin();
    ((offset = std::copy_n(strings, sizeof(strings) - 1, offset)), ...);

    buffer[totalSize - 1] = '\0';

    return AutomaticDurationString(buffer);
}

template<AutomaticDurationString Data>
constexpr auto operator""_ads()
{
    return Data;
}
} // namespace ctoAssetsRTIS