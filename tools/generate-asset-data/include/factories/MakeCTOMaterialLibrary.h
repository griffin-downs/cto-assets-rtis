// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <exception>
#include <format>
#include <typeinfo>
#include <vector>

#include <assimp/scene.h>
#include <assimp/types.h>

#include "converters/AssimpSTLConverters.h"
#include "data-types/CTOMaterialLibrary.h"
#include "utilities/GetMaterialProperty.h"


namespace ctoAssetsRTIS
{
auto makeCTOMaterialLibrary(const aiScene& scene)
{
    return
        CTOMaterialLibrary
        {
            .definitions =
            [&]
            {
                auto definitions =
                    std::vector<CTOMaterialLibrary::Definition>{};
                definitions.reserve(scene.mNumMaterials);

                for (auto i = size_t{}; i < scene.mNumMaterials; i++)
                {
                    const auto& material = *scene.mMaterials[i];

                    definitions.push_back({
                        .name =
                            getMaterialProperty<
                                aiString
                            >(material, AI_MATKEY_NAME),
                        .diffuseColor =
                            getMaterialProperty<
                                aiColor3D
                            >(material, AI_MATKEY_COLOR_DIFFUSE)
                    });
                }

                return definitions;
            }()
        };
}
} // namespace ctoAssetsRTIS
