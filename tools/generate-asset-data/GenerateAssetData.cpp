// =============================================================================
// Copyright (C) 2025, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ArgumentBuffers.h"
#include "AutomaticDurationString.h"
#include "factories/MakeCTOMaterialLibrary.h"
#include "factories/MakeCTOMesh.h"
#include "formatting/FormatCTOMaterialLibrary.h"
#include "formatting/FormatCTOMesh.h"

namespace ctoAssetsRTIS
{
namespace fs = std::filesystem;

template<typename T>
struct File
{
    std::string name;
    T contents;
};

struct GenerateAssetFiles
{
    struct Configuration
    {
        fs::path outputDirectory;
    };
    GenerateAssetFiles(Configuration configuration)
    : outputDirectory{ configuration.outputDirectory }
    {
        fs::create_directories(this->outputDirectory);
    }

    auto operator()(fs::path inputFile) const
    {
        auto importer = Assimp::Importer();

        const auto* scene =
        [&]
        {
            const auto inputFileString = inputFile.generic_string();

            static constexpr auto importFlags =
                aiProcessPreset_TargetRealtime_MaxQuality;

            const auto* scene =
                importer.ReadFile(inputFileString.data(), importFlags);

            const auto assertScene =
            [&]
            {
                if (!scene)
                {
                    return false;
                }

                if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
                {
                    return false;
                }

                if (!scene->mRootNode)
                {
                    return false;
                }

                return true;
            };

            if (!assertScene())
            {
                throw
                    std::runtime_error(
                        std::format(
                            "Unable to load scene for file: {}\n"
                            "Import Error:\n\t{}",
                            inputFileString,
                            importer.GetErrorString()));
            }

            return scene;
        }();

        const auto outputFile = outputDirectory / inputFile.filename();

        return
            std::make_tuple(
                File
                {
                    .name =
                        fs::path(outputFile)
                            .replace_extension(".cto.obj")
                            .generic_string(),
                    .contents = makeCTOMesh(*scene)
                },
                File
                {
                    .name =
                        fs::path(outputFile)
                            .replace_extension(".cto.mtl")
                            .generic_string(),
                    .contents = makeCTOMaterialLibrary(*scene)
                }
                // File
                // {
                //     .name =
                //     [&]
                //     {
                //         const auto inputFilename =
                //             inputFile
                //                 .filename()
                //                 .string();

                //         return
                //             path(inputFile)
                //                 .replace_filename(inputFilename + "_normal")
                //                 .replace_extension(".cto.map")
                //     }(),
                //     .contents = CTONormalMap(scene)
                // }
                );
    }

    fs::path outputDirectory;
};
} // namespace ctoAssetsRTIS

int main()
{
    using namespace ctoAssetsRTIS;

    try
    {
        const auto generateAssetFiles =
            GenerateAssetFiles({
                .outputDirectory =
                []
                {
                    auto argumentBuffer =
                        ArgumentBuffer<"OUTPUT_DIRECTORY"_ads>{};

                    argumentBuffer.readLine();

                    argumentBuffer.forward();

                    return fs::path(argumentBuffer.getValueView());
                }()
            });

        auto inputFileBuffer = ArgumentBuffer<"INPUT_FILE"_ads>{};
        auto outFile = std::ofstream();
        outFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);

        while (inputFileBuffer.readLine())
        {
            const auto assetFiles =
                generateAssetFiles({ inputFileBuffer.getValueView() });

            [&]<size_t... Indices>(std::index_sequence<Indices...>)
            {
                const auto processFile =
                [&](const auto& file)
                {
                    outFile.open(file.name);
                    outFile
                        << file.contents
                        << std::endl;

                    outFile.close();

                    std::cout
                        << std::format("INPUT_FILE={}", file.name)
                        << std::endl;
                };

                (processFile(std::get<Indices>(assetFiles)), ...);
            }(std::make_index_sequence<
                std::tuple_size<decltype(assetFiles)>::value
            >{});
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
