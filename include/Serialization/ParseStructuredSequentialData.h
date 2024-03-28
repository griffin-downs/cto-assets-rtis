// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <string_view>
#include <tuple>
#include <utility>

#include "AutomaticDurationString.h"

namespace ctoAssetsRTIS
{
constexpr float parseFloat(std::string_view str)
{
    int sign = 1;

    if (!str.empty() && str[0] == '-')
    {
        sign = -1;
        str.remove_prefix(1);
    }

    float result = 0.0f;

    float divisor = 1.0f;
    bool decimalEncountered = false;
    for (auto c : str)
    {
        if (c == '.') {
            decimalEncountered = true;
            continue;
        }

        const int digit = c - '0';
        if (digit < 0 || digit > 9) {
            break;
        }

        if (decimalEncountered)
        {
            divisor *= 0.1f;
            result += digit * divisor;
            continue;
        }

        result = result * 10.0f + digit;
    }

    return result * sign;
}

template<auto _AutomaticDurationString>
struct Sequence
{
private:
    static constexpr auto buffer =
        makeAutomaticDurationString(_AutomaticDurationString.data, " ");

public:
    static constexpr auto view = buffer.toStringView();
};

template<
    typename SequenceType,
    template<typename...> typename ResultTypeTemplate,
    typename... Rules
>
struct Rule
{
    using Sequence = SequenceType;

    template<typename... Arguments>
    using ResultType = ResultTypeTemplate<Arguments...>;

    static constexpr std::tuple<Rules...> subRules;
};

template<auto Data, typename TopLevelRule>
class ParseStructuredSequentialData
{
private:
    static constexpr auto dataView = Data.toStringView();

    template<size_t Begin, size_t Length>
    struct LogicalBlockRange
    {
        size_t begin = Begin;
        size_t length = Length;
    };

    template<typename CurrentRule, LogicalBlockRange Range>
    class ParseLogicalBlock
    {
    private:
        using Sequence = CurrentRule::Sequence;
        static constexpr auto subRules = CurrentRule::subRules;
        static constexpr auto subRulesCount =
            std::tuple_size<decltype(CurrentRule::subRules)>::value;

        static constexpr auto logicalBlockView =
            dataView.substr(Range.begin, Range.length);

        static constexpr auto elementCount =
        []
        {
            auto count = size_t{};
            for (
                auto offset = logicalBlockView.find(Sequence::view);
                offset != logicalBlockView.npos;
                offset = logicalBlockView.find(Sequence::view, offset + 1))
            {
                count++;
            }

            return count;
        }();

        template<size_t SubRangeIndex>
        static constexpr auto getLogicalBlockAbsoluteSubRange()
        {
            constexpr auto subRangeOffset =
            [&]
            {
                auto offset = logicalBlockView.find(Sequence::view);
                for (auto i = size_t{}; i < SubRangeIndex; i++)
                {
                    offset = logicalBlockView.find(Sequence::view, offset + 1);
                }

                return offset;
            }();
            static_assert(
                subRangeOffset != logicalBlockView.npos,
                "No items found.");

            constexpr auto nextOffset =
                logicalBlockView.find(Sequence::view, subRangeOffset + 1);

            constexpr auto subRangeLength =
                nextOffset == logicalBlockView.npos
                    ? Range.length - subRangeOffset
                    : nextOffset - subRangeOffset;

            return LogicalBlockRange<
                Range.begin + subRangeOffset,
                subRangeLength
            >{};
        };

        template<LogicalBlockRange SubRange>
        static constexpr auto parseStructure()
        {
            static constexpr auto fields =
            [&]<size_t... RuleIndices>
            (std::index_sequence<RuleIndices...>)
            {
                return
                    std::make_tuple(
                        ParseLogicalBlock<
                            typename std::tuple_element<
                                RuleIndices,
                                decltype(subRules)
                            >::type,
                            SubRange
                        >::value...);
            }(std::make_index_sequence<subRulesCount>());

            const auto id =
            [&]
            {
                constexpr auto rawElementContents =
                    dataView.substr(SubRange.begin + Sequence::view.size());

                constexpr auto idLength = rawElementContents.find('\n') + 1;

                auto id = AutomaticDurationString<idLength>{};

                std::copy_n(
                    rawElementContents.begin(),
                    idLength,
                    id.data);

                return id;
            }();

            return
                std::apply(
                    [&](auto&&... arguments)
                    {
                        return
                            typename CurrentRule::template
                            ResultType<decltype(id), decltype(arguments)...>
                            {
                                id,
                                arguments...
                            };
                    },
                    fields);
        }

        static constexpr auto parseValueRange()
        {
            using ResolvedResultType =
                typename CurrentRule::template ResultType<void>;

            auto result =
                std::array<
                    ResolvedResultType,
                    elementCount
                >{};

            auto elementOffset = logicalBlockView.find(Sequence::view);
            for (auto i = size_t{}; i < elementCount; i++)
            {
                auto rawElementContents =
                [&]
                {
                    const auto begin = elementOffset + Sequence::view.size();

                    const auto newLineOffset =
                        logicalBlockView.find('\n', begin);

                    const auto length =
                        newLineOffset == logicalBlockView.npos
                            ? logicalBlockView.npos
                            : newLineOffset - begin;

                    return logicalBlockView.substr(begin, length);
                }();

                auto tokenOffset = size_t{};
                constexpr auto tokenCount = result[0].size();
                for (auto j = size_t{}; j < tokenCount; j++)
                {
                    const auto spaceOffset =
                        rawElementContents.find(' ', tokenOffset);

                    const auto token =
                        rawElementContents.substr(
                            tokenOffset,
                            spaceOffset - tokenOffset);

                    result[i][j] = parseFloat(token);

                    tokenOffset =
                        spaceOffset == rawElementContents.npos
                            ? rawElementContents.npos
                            : spaceOffset + 1;
                }

                elementOffset =
                    logicalBlockView.find(
                        Sequence::view,
                        elementOffset + 1);
            }

            return result;
        }

    public:
        static constexpr auto value =
        []
        {
            if constexpr (subRulesCount > 0)
            {
                return
                []<size_t... Indices>
                (std::index_sequence<Indices...>)
                {
                    return
                        std::make_tuple(
                            parseStructure<
                                getLogicalBlockAbsoluteSubRange<Indices>()
                            >()...);
                }(std::make_index_sequence<elementCount>{});
            }
            else
            {
                return parseValueRange();
            }
        }();
    };

public:
    static constexpr auto value =
        ParseLogicalBlock<
            TopLevelRule,
            LogicalBlockRange<0, std::string_view::npos>{}
        >::value;
};

namespace WavefrontObjSchema
{
    template<typename...>
    using Vertex = std::array<float, 3>;

    template<typename...>
    using Face = std::array<unsigned int, 3>;

    template<typename IdString, typename Faces>
    struct UseMaterialDirective
    {
        IdString id;
        Faces faces;
    };

    template<
        typename IdString,
        typename Vertices,
        typename UseMaterialDirectives
    >
    struct Object
    {
        IdString id;
        Vertices vertices;
        UseMaterialDirectives useMaterialDirectives;
    };

    using Ruleset =
        Rule<
            Sequence<"o"_ads>,
            Object,
            Rule<Sequence<"v"_ads>, Vertex>,
            Rule<
                Sequence<"usemtl"_ads>,
                UseMaterialDirective,
                Rule<Sequence<"f"_ads>, Face>
            >
        >;
}

template<typename ModelData>
using ParseWavefrontObj =
    ParseStructuredSequentialData<
        ModelData::value,
        WavefrontObjSchema::Ruleset
    >;

namespace WavefrontMtlSchema
{
    template<typename...>
    using DiffuseColor = std::array<float, 3>;

    template<typename IdString, typename DiffuseColors>
    struct Definition
    {
        IdString id;
        DiffuseColors diffuseColors;
    };

    using Ruleset =
        Rule<
            Sequence<"newmtl"_ads>,
            Definition,
            Rule<Sequence<"Kd"_ads>, DiffuseColor>
        >;
}

template<typename MaterialLibraryData>
using ParseWavefrontMtl =
    ParseStructuredSequentialData<
        MaterialLibraryData::value,
        WavefrontMtlSchema::Ruleset
    >;
} // namespace ctoAssetsRTIS
