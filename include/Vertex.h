// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <array>


namespace ctoAssetsRTIS
{
struct VertexAttribute
{
    GLuint index;
    GLint elementCount;
    GLenum elementType;
    size_t offset;
};

template<typename... Traits>
class Vertex
{
private:
    template<size_t Index>
    static constexpr auto calculateOffset()
    {
        return accumulateOffset<0, Index, Traits...>();
    }

    template<size_t Result, size_t Index, typename First, typename... Rest>
    static constexpr auto accumulateOffset()
    {
        if constexpr (Index == 0)
        {
            return Result;
        }
        else
        {
            return accumulateOffset<
                Result + (First::elementCount * First::elementSize),
                Index - 1,
                Rest...
            >();
        }
    }

    template<size_t N>
    struct nthTrait
    {
        using Trait = std::tuple_element<N, std::tuple<Traits...>>::type;
        static constexpr auto elementType = Trait::elementType;
        static constexpr auto elementCount = Trait::elementCount;
        static constexpr auto elementSize = Trait::elementSize;
    };

public:
    static constexpr GLsizei stride =
        (... + (Traits::elementCount * Traits::elementSize));

    using VertexAttributes = std::array<VertexAttribute, sizeof...(Traits)>;
    static constexpr auto attributes =
    []
    {
        return
        []<size_t... Indices>(std::index_sequence<Indices...>)
        {
            return std::array<VertexAttribute, sizeof...(Traits)>
            {
                {
                    VertexAttribute
                    {
                        .index = static_cast<GLuint>(Indices),
                        .elementCount = nthTrait<Indices>::elementCount,
                        .elementType = nthTrait<Indices>::elementType,
                        .offset = calculateOffset<Indices>()
                    }...
                }
            };
        }(std::index_sequence_for<Traits...>{});
    }();

    static constexpr auto elementCount = (Traits::elementCount + ...);
    std::array<GLfloat, elementCount> data;

    constexpr Vertex(const Traits&... traits) : data{}
    {
        auto offset = this->data.begin();
        ((offset =
            std::copy(
                traits.data.begin(),
                traits.data.end(),
                offset)),
            ...);
    }

    constexpr Vertex(): data{} {}

    constexpr const std::array<GLfloat, elementCount>& getData() const
    {
        return this->data;
    }
};

template<typename VertexType, size_t VertexCount>
constexpr auto flattenVertices(
    const std::array<VertexType, VertexCount>& vertices)
{
    auto flattenedVertices =
        std::array<GLfloat, VertexType::elementCount * VertexCount>{};

    auto offset = size_t{};
    for (const auto& vertex : vertices)
    {
        const auto& vertexData = vertex.getData();

        std::copy(
            std::begin(vertexData),
            std::end(vertexData),
            std::begin(flattenedVertices) + offset);

        offset += vertexData.size();
    }

    return flattenedVertices;
}

template<size_t ElementCount>
struct VertexTrait
{
    std::array<GLfloat, ElementCount> data;

    static constexpr auto elementType = GL_FLOAT;
    static constexpr auto elementCount = ElementCount;
    static constexpr auto elementSize = sizeof(GLfloat);

    constexpr VertexTrait() {}

    template<typename... Args>
    constexpr VertexTrait(Args&&... args)
    : data({ std::forward<Args>(args)... })
    {
    }

    constexpr const std::array<GLfloat, ElementCount>& getData() const
    {
        return this->data;
    }
};

struct ColorRGBA : public VertexTrait<4> {};
struct Position : public VertexTrait<3> {};

struct VertexPositionRGBA : public Vertex<Position, ColorRGBA>
{
    using Vertex<Position, ColorRGBA>::Vertex;
};
} // namespace ctoAssetsRTIS
