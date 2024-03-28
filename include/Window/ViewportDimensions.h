// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <type_traits>


namespace ctoAssetsRTIS
{
struct ViewportDimensions
{
private:
    int width;
    int height;

public:
    struct Dimensions
    {
        int width;
        int height;
    };
    ViewportDimensions(Dimensions dimensions)
    : width{ dimensions.width > 0 ? dimensions.width : 0 }
    , height{ dimensions.height > 0 ? dimensions.height : 1 }
    {
    }

    int getWidth() const { return this->width; }
    int getHeight() const { return this->height; }

#ifdef __EMSCRIPTEN__
    static auto fromCanvasSize()
    {
        double width, height;
        emscripten_get_element_css_size(
            "#canvas-container",
            &width,
            &height);

        return ViewportDimensions({
            .width = (int)width,
            .height = (int)height
        });
    }
#endif

    static auto getDefault()
    {
#ifdef __EMSCRIPTEN__
        return fromCanvasSize();
#else
        return ViewportDimensions({ .width = 800, .height = 600 });
#endif
    }

    template<size_t N>
    friend constexpr auto get(const ViewportDimensions&) -> decltype(auto);
};

template<size_t N>
constexpr auto get(const ViewportDimensions& dimensions) -> decltype(auto)
{
    if constexpr (N == 0) return (dimensions.width);
    else if constexpr (N == 1) return (dimensions.height);
    else static_assert(N < 2, "Index out of range for ViewportDimensions");
}
} // namespace ctoAssetsRTIS

namespace std
{
template<>
class tuple_size<ctoAssetsRTIS::ViewportDimensions>
: public integral_constant<size_t, 2>
{
};

template<size_t N>
class tuple_element<N, ctoAssetsRTIS::ViewportDimensions>
{
public:
    using type =
        decltype(
            get<N>(
                declval<ctoAssetsRTIS::ViewportDimensions>()));
};
} // namespace std
