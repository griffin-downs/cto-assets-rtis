// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
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
auto makeCTOMesh(const aiScene& scene)
{
    auto mesh = CTOMesh{
        .name = "MergedMesh",
        .vertices =
        [&]
        {
            auto vertices = std::vector<CTOMesh::Vertex>{};

            // Preallocate with heuristic
            size_t totalVertices = 0;
            for (auto i = size_t{}; i < scene.mNumMeshes; i++)
                totalVertices += scene.mMeshes[i]->mNumVertices;
            vertices.reserve(totalVertices);

            return vertices;
        }(),
        .materialDirectives = {}
    };

    auto materialLookup = std::map<std::string, size_t>{};
    auto indexOffset = size_t{};

    for (auto meshIndex = size_t{}; meshIndex < scene.mNumMeshes; meshIndex++)
    {
        const auto& assimpMesh = *scene.mMeshes[meshIndex];
        const auto& material = *scene.mMaterials[assimpMesh.mMaterialIndex];
        const auto materialName =
            getMaterialProperty<aiString>(material, AI_MATKEY_NAME);

        // Add vertices for this mesh
        for (auto i = size_t{}; i < assimpMesh.mNumVertices; i++)
        {
            mesh.vertices.push_back({
                .position =
                [&]
                {
                    assert(assimpMesh.HasPositions());
                    const auto& v = assimpMesh.mVertices[i];
                    return std::to_array<float>({v.x, v.y, v.z});
                }(),
                .texture = std::nullopt,
                .normal =
                [&]() -> std::optional<std::array<float, 3>>
                {
                    if (!assimpMesh.HasNormals())
                        return std::nullopt;

                    const auto& n = assimpMesh.mNormals[i];
                    return std::to_array<float>({n.x, n.y, n.z});
                }()
            });
        }

        // Ensure a material directive exists
        const auto directiveIndex =
        [&]() -> size_t
        {
            const auto it = materialLookup.find(materialName);
            if (it != materialLookup.end())
                return it->second;

            const auto newIndex = mesh.materialDirectives.size();
            mesh.materialDirectives.push_back({
                .name = materialName,
                .faces = {}
            });
            materialLookup[materialName] = newIndex;
            return newIndex;
        }();

        // Add faces to the directive
        auto& faces = mesh.materialDirectives[directiveIndex].faces;
        faces.reserve(faces.size() + assimpMesh.mNumFaces);

        for (auto i = size_t{}; i < assimpMesh.mNumFaces; i++)
        {
            const auto& f = assimpMesh.mFaces[i];
            assert(f.mNumIndices == 3 && "Expected triangulated faces");

            faces.push_back({
                .vertexIndices =
                    std::to_array<unsigned>({
                        static_cast<unsigned>(f.mIndices[0] + indexOffset),
                        static_cast<unsigned>(f.mIndices[1] + indexOffset),
                        static_cast<unsigned>(f.mIndices[2] + indexOffset)
                    }),
                .textureIndices = std::nullopt
            });
        }

        indexOffset += assimpMesh.mNumVertices;
    }

    return mesh;
}
} // namespace ctoAssetsRTIS
