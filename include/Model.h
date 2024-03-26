// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include "MaterialLibrary.h"
#include "Mesh.h"

namespace ctoAssetsRTIS
{
struct Model
{
    const Mesh mesh;
    const MaterialLibrary materialLibrary;
};
} // namespace ctoAssetsRTIS
