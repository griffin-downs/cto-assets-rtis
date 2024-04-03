// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#include <array>
#include <exception>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>

#include "AutomaticDurationString.h"
#include "ArgumentBuffers.h"


namespace ctoAssetsRTIS
{
void stringifyFile(
    std::string_view inputFile,
    std::string_view outputFile,
    std::string_view typeName)
{

    try
    {
        auto inFile = std::ifstream();
        inFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        inFile.open(inputFile.data());

        auto outFile = std::ofstream();
        outFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        outFile.open(outputFile.data());

        outFile
            << "#pragma once\n\n"
            << "#include \"AutomaticDurationString.h\"\n\n"
            << "namespace ctoAssetsRTIS\n"
            << "{\n"
            << "namespace fileContents\n"
            << "{\n"
            << "struct " << typeName << "\n"
            << "{\n"
            << "    static constexpr auto value =\n"
            << "        makeAutomaticDurationString(R\"DELIMITER(";

        {
            static constexpr auto bufferSizeBytes = size_t{ 8 * 1024 };
            static auto buffer = std::array<char, bufferSizeBytes>{};
            static constexpr auto maxBatchSizeBytes = size_t{ 50000 };
            {
                static constexpr size_t maxLengthStringLiteral = 65536;
                static_assert(
                    bufferSizeBytes + maxBatchSizeBytes
                        <= maxLengthStringLiteral);
            }

            auto batchSizeBytes = size_t{};
            auto batchReadAndWrite =
            [&]()
            {
                inFile.read(buffer.data(), buffer.size());

                const auto bytesRead = inFile.gcount();
                if (bytesRead > 0)
                {
                    outFile.write(buffer.data(), bytesRead);
                }

                if (inFile.eof())
                {
                    return false;
                }

                if (!inFile.good())
                {
                    throw std::runtime_error("Error while reading the file.");
                }

                batchSizeBytes += bytesRead;
                if (batchSizeBytes > maxBatchSizeBytes)
                {
                    outFile
                        << ")DELIMITER\",\n"
                        << "        R\"DELIMITER(";

                    batchSizeBytes = 0;
                }

                return true;
            };
            while (batchReadAndWrite());
        }

        outFile
            << ")DELIMITER\");\n"
            << "};\n"
            << "} // namespace fileContents\n"
            << "} // namespace ctoAssetsRTIS\n";
    }
    catch (const std::exception& e)
    {
        throw
            std::runtime_error(
                std::format(
                    "Error reading/writing files.\n"
                    "Arguments:\n\t{}\n\t{}\n\t{}\n"
                    "Exception details:\n\t{}",
                    inputFile,
                    outputFile,
                    typeName,
                    e.what()));
    }
}
} // namespace ctoAssetsRTIS

int main()
{
    using namespace ctoAssetsRTIS;

    try
    {
        auto argumentBuffers =
            ArgumentBuffers<
                "INPUT_FILE"_ads,
                "OUTPUT_FILE"_ads,
                "TYPE_NAME"_ads
            >{};

        while (argumentBuffers.readLines())
        {
            auto [
                inputFile,
                outputFile,
                typeName
            ] = argumentBuffers.getValueViews();

            stringifyFile(inputFile, outputFile, typeName);

            argumentBuffers.forward<"OUTPUT_FILE"_ads, "TYPE_NAME"_ads>();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}