// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>
#include <tuple>
#include <string_view>
#include <type_traits>
#include <utility>

#include "Serialization/Deserialize.h"
#include "Serialization/ParseStructuredSequentialData.h"
#include "Vertex.h"


namespace ctoAssetsRTIS
{
struct MeshData
{
    struct MaterialChunk
    {
        std::string_view name;
        size_t offset;
        size_t count;
    };

    const std::span<const GLfloat> vertices;
    const std::span<const GLuint> faceIndices;
    const std::span<const MaterialChunk> materialChunks;
    const std::span<const VertexAttribute> vertexAttributes;
    const GLuint vertexStride;
};

class Mesh;

template<typename StringProvider>
class CompileTimeDeserialize<Mesh, StringProvider>
{
private:
    static constexpr auto& parsedObjectModels =
        ParseWavefrontObj<StringProvider>::value;

    static constexpr auto extractVertices()
    {
        return
            std::apply(
                [](auto&&... parsedObjectModels)
                {
                    return std::make_tuple(parsedObjectModels.vertices...);
                },
                parsedObjectModels);
    }

    static constexpr auto extractUseMaterialDirectives()
    {
        return
        [&]<size_t... Indices>(std::index_sequence<Indices...>)
        {
            return
                std::tuple_cat(
                    std::get<Indices>(parsedObjectModels)
                        .useMaterialDirectives...);
        }(std::make_index_sequence<
            std::tuple_size<
                typename std::decay<decltype(parsedObjectModels)>::type
            >::value
        >{});
    }

    static constexpr auto extractFaces()
    {
        return
            std::apply(
                [](auto&&... useMaterialDirectives)
                {
                    return std::make_tuple(useMaterialDirectives.faces...);
                },
                extractUseMaterialDirectives());
    }

    template<typename Tuple>
    static constexpr auto flatMapTupleOfArrays(
        const Tuple& tuple,
        auto convertElement)
    {
        constexpr auto totalSize =
        [&]<size_t... Indices>(std::index_sequence<Indices...>)
        {
            return
                (std::tuple_size<
                    typename std::tuple_element<Indices, Tuple>::type
                >::value + ...);
        }(std::make_index_sequence<std::tuple_size<Tuple>::value>{});


        using InputElementType = std::tuple_element<0, Tuple>::type::value_type;
        using ResultElementType =
            std::invoke_result<
                decltype(convertElement),
                InputElementType
            >::type;

        auto result = std::array<ResultElementType, totalSize>{};

        [&]<size_t... Indices>(std::index_sequence<Indices...>)
        {
            auto copy =
            [&, offset = size_t{}](const auto element) mutable
            {
                result[offset++] = element;
            };

            const auto copyAndConvertArrayElements =
            [&](const auto& array)
            {
                for (const auto& element : array)
                {
                    copy(convertElement(element));
                }
            };

            ((copyAndConvertArrayElements(std::get<Indices>(tuple))), ...);
        }(std::make_index_sequence<
            std::tuple_size<Tuple>::value
        >{});

        return result;
    }

public:
    using VertexType = Vertex<Position>;
    static constexpr auto vertices =
    []
    {
        return
            flattenVertices(
                flatMapTupleOfArrays(
                extractVertices(),
                [](auto vertex)
                {
                    return
                        VertexType(
                            Position({
                                vertex[0],
                                vertex[1],
                                vertex[2]
                            }));
                }));
    }();

    static constexpr auto faceIndices =
    []
    {
        const auto facesArray =
            flatMapTupleOfArrays(
                extractFaces(),
                [](auto face)
                {
                    for (auto vertexIndex : face)
                    {
                        vertexIndex--;
                    }

                    return face;
                });

        constexpr auto facesArraySize =
            std::tuple_size<decltype(facesArray)>::value;

        constexpr auto faceSize =
            std::tuple_size<typename decltype(facesArray)::value_type>::value;

        auto result =
            std::array<
                unsigned,
                facesArraySize * faceSize
            >{};

        for (auto i = size_t{}; i < facesArraySize; i++)
        {
            const auto& face = facesArray[i];
            const auto offset = i * faceSize;

            result[offset] = face[0] - 1;
            result[offset + 1] = face[1] - 1;
            result[offset + 2] = face[2] - 1;
        }

        return result;
    }();

    static constexpr auto materialChunks =
    []
    {
        constexpr auto useMaterialDirectives = extractUseMaterialDirectives();

        constexpr auto useMaterialDirectivesCount =
            std::tuple_size<decltype(useMaterialDirectives)>::value;

        auto result =
            std::array<
                MeshData::MaterialChunk,
                useMaterialDirectivesCount
            >{};

        static constexpr auto idStringCache =
            std::apply(
                [](auto&&... useMaterialDirectives)
                {
                    return
                        std::make_tuple(
                            AutomaticDurationString(
                                useMaterialDirectives.id)...);
                },
                useMaterialDirectives);

        [&]<size_t... Indices>(std::index_sequence<Indices...>)
        {
            auto offset = size_t{};
            const auto makeChunk =
            [&](const auto& useMaterialDirective, const auto& idString)
            {
                const auto facesCount = useMaterialDirective.faces.size();

                using FaceType =
                    typename std::decay<
                        decltype(useMaterialDirective.faces)
                    >::type::value_type;

                const auto faceIndexCount = std::tuple_size<FaceType>::value;

                const auto count = facesCount * faceIndexCount;

                const auto chunk =
                    MeshData::MaterialChunk
                    {
                        .name = idString.toStringView(),
                        .offset = offset,
                        .count = count
                    };

                offset += count;

                return chunk;
            };

            ((result[Indices] =
                makeChunk(
                    std::get<Indices>(useMaterialDirectives),
                    std::get<Indices>(idStringCache))), ...);
        }(std::make_index_sequence<useMaterialDirectivesCount>{});

        return result;
    }();

public:
    static constexpr auto value =
        MeshData
        {
            .vertices = vertices,
            .faceIndices = faceIndices,
            .materialChunks = materialChunks,
            .vertexAttributes = VertexType::attributes,
            .vertexStride = VertexType::stride
        };
};
} // namespace ctoAssetsRTIS
