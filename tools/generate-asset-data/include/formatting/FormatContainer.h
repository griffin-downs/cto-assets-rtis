// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>
#include <cassert>
#include <format>
#include <ostream>
#include <vector>


namespace ctoAssetsRTIS
{
namespace
{
template<typename Container>
auto& formatContainer(
    std::ostream& ostream,
    const Container& container,
    const char* elementDelimiter)
{
    assert(!container.empty());

    std::copy(
        container.begin(),
        container.end() - 1,
        std::ostream_iterator<
            typename Container::value_type
        >(ostream, elementDelimiter));

    return ostream << container.back();
};
} // unnamed namespace

namespace spaceDelimited::array
{
template<typename T, size_t N>
auto& operator<<(std::ostream& ostream, const std::array<T, N>& array)
{
    return formatContainer(ostream, array, " ");
}
} // namespace spaceDelimited::array

namespace newlineDelimited::vector
{
template<typename T>
auto& operator<<(std::ostream& ostream, const std::vector<T>& vector)
{
    if (vector.empty())
    {
        return ostream;
    }

    return formatContainer(ostream, vector, "\n");
}
} // namespace newlineDelimited::vector
} // namespace ctoAssetsRTIS
