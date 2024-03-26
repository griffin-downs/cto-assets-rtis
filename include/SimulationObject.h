// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include "Model.h"
#include "Transform.h"


namespace ctoAssetsRTIS
{
struct SimulationObject
{
    const Model& model;
    Transform transform;
};
} // namespace ctoAssetsRTIS