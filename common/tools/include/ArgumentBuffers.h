// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================

#pragma once

#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>

#include "AutomaticDurationString.h"

namespace ctoAssetsRTIS
{
template<AutomaticDurationString Key>
class ArgumentBuffer
{
public:
    static constexpr auto keyView = Key.toStringView();

private:
    static constexpr auto delimiter = "=";

    static constexpr auto keyDelimiterLength =
    []
    {
        constexpr auto delimiterLength =
            std::char_traits<char>::length(delimiter);

        return keyView.size() + delimiterLength;
    }();


public:
    auto readLine()
    {
        if (std::getline(std::cin, this->buffer))
        {
            validateBufferContents();
            return true;
        }

        if (!std::cin.fail() || std::cin.eof())
        {
            return false;
        }

        throw std::runtime_error(
            std::format(
                "Unexpected failure attempting to read parameter: {}",
                keyView));
    }

    auto getValueView() const
    {
        return std::string_view{ this->buffer.data() + keyDelimiterLength };
    }

    const auto& getBuffer() const
    {
        return this->buffer;
    }

private:
    std::string buffer;

    void validateBufferContents() const
    {
        const auto rest = this->buffer.substr(keyView.size());
        if (this->buffer.starts_with(keyView) && rest.starts_with(delimiter))
        {
            return;
        }

        throw std::runtime_error(
            std::format(
                "Buffer input not well formed:\n"
                "\tExpected buffer beginning:\n\t\t{}\n"
                "\tBuffer contents:\n\t\t{}\n",
                std::format("{}{}", keyView, delimiter),
                this->buffer));
    }
};

template<AutomaticDurationString... ArgumentNames>
class ArgumentBuffers
{
private:
    template<auto ArgumentName>
    auto& getArgumentBuffer() const
    {
        return
            std::get<
                ArgumentBuffer<ArgumentName>
            >(this->argumentBuffers);
    }

public:
    template<AutomaticDurationString... ForwardingArgumentNames>
    void forward() const
    {
        ((std::cout
            << getArgumentBuffer<ForwardingArgumentNames>().getBuffer()
            << '\n'), ...);

        std::cout << std::endl;
    }

    auto readLines()
    {
        return
        [&]<size_t... Indices>(std::index_sequence<Indices...>)
        {
            return
                ((std::get<Indices>(this->argumentBuffers).readLine()) && ...);
        }(std::make_index_sequence<
            std::tuple_size<decltype(this->argumentBuffers)>::value
        >{});
    }

    auto getValueViews() const
    {
        return
        [&]<size_t... Indices>(std::index_sequence<Indices...>)
        {
            return
                std::make_tuple(
                    std::get<Indices>(this->argumentBuffers).getValueView()...);
        }(std::make_index_sequence<
            std::tuple_size<decltype(this->argumentBuffers)>::value
        >{});
    }

private:
    std::tuple<ArgumentBuffer<ArgumentNames>...> argumentBuffers;
};
} // namespace ctoAssetsRTIS
