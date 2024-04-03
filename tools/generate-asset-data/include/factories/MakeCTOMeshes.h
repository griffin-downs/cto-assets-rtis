// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#pragma once

#include <cassert>
#include <map>
#include <vector>

#include <assimp/scene.h>

#include "data-types/CTOMesh.h"
#include "utilities/GetMaterialProperty.h"


namespace ctoAssetsRTIS
{
auto makeCTOMeshes(const aiScene& scene)
{
    auto meshes = std::vector<CTOMesh>{};
    meshes.reserve(scene.mNumMeshes);

    for (auto meshIndex = size_t{}; meshIndex < scene.mNumMeshes; meshIndex++)
    {
        const auto& mesh = *scene.mMeshes[meshIndex];

        meshes.push_back({
            .name = assimpSTLConverters::convert(mesh.mName),
            .materialName =
            [&]
            {
                const auto& material = *scene.mMaterials[mesh.mMaterialIndex];
                return getMaterialProperty<aiString>(material, AI_MATKEY_NAME);
            }(),
            .vertices =
            [&]
            {
                auto vertices = std::vector<CTOMesh::Vertex>{};
                vertices.reserve(mesh.mNumVertices);

                for (auto i = size_t{}; i < mesh.mNumVertices; i++)
                {
                    vertices.push_back({
                        .position =
                        [&]
                        {
                            assert(mesh.HasPositions());

                            const auto& assimpVertex = mesh.mVertices[i];
                            return
                                std::to_array<float>({
                                    assimpVertex.x,
                                    assimpVertex.y,
                                    assimpVertex.z
                                });
                        }(),
                        .texture = std::nullopt,
                        .normal =
                        [&]
                        {
                            assert(mesh.HasNormals());

                            const auto& assimpNormal = mesh.mNormals[i];
                            return
                                std::to_array<float>({
                                    assimpNormal.x,
                                    assimpNormal.y,
                                    assimpNormal.z
                                });
                        }(),
                    });
                }

                return vertices;
            }(),
            .faces =
            [&]
            {
                auto faces = std::vector<CTOMesh::Face>{};
                faces.reserve(mesh.mNumFaces);

                for (auto i = size_t{}; i < mesh.mNumFaces; i++)
                {
                    const auto& assimpFace = mesh.mFaces[i];

                    faces.push_back({
                        .vertexIndices =
                            std::to_array({
                                assimpFace.mIndices[0],
                                assimpFace.mIndices[1],
                                assimpFace.mIndices[2]
                            })
                    });
                }

                return faces;
            }()
        });
    }

    return meshes;
}
} // namespace ctoAssetsRTIS
