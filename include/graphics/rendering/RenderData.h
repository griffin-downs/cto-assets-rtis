// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <optional>

#include "utils/Constrained.h"


namespace ctoAssetsRTIS
{
struct RenderData
{
    struct Faceted
    {
        NormalizedFloat strength{ 1.0f };
        AtLeast1Float power{ 1.0f };
    };

    std::optional<Faceted> faceted;
    std::optional<NormalizedFloat> bloom{};
};
} // namespace ctoAssetsRTIS