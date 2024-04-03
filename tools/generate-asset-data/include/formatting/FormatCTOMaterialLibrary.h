// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <ostream>

#include "FormatContainer.h"
#include "data-types/CTOMaterialLibrary.h"


namespace ctoAssetsRTIS
{
using namespace spaceDelimited::array;
using namespace newlineDelimited::vector;

auto& operator<<(
    std::ostream& ostream,
    const CTOMaterialLibrary::Definition& definition)
{
    const auto& [name, diffuseColor] = definition;

    return
        ostream
            << "newmtl " << name << '\n'
            << "Kd " << diffuseColor;
}

auto& operator<<(
    std::ostream& ostream,
    const CTOMaterialLibrary& materialLibrary)
{
    const auto& [ definitions ] = materialLibrary;

    return ostream << definitions;
}
} // namespace ctoAssetsRTIS
