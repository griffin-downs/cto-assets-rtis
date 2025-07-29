// =============================================================================
// Copyright (C) 2024, Griffin Downs. All rights reserved.
// This file is part of cto-assets-rtis. See LICENSE.md for details.
// =============================================================================


#include <array>
#include <exception>
#include <filesystem>
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
namespace fs = std::filesystem;

class StringifyFiles
{
public:
    struct Configuration
    {
        fs::path outputDirectory;
    };
    StringifyFiles(Configuration configuration)
    : outputDirectory{ configuration.outputDirectory }
    {
        fs::create_directories(this->outputDirectory);
    }

private:
    static auto toCamelCase(std::string_view input)
    {
        auto result = std::string{};
        result.reserve(input.size());

        auto capitalizeNext = true;

        for (auto c : input)
        {
            if (c == '_' || c == '-' || c == '.')
            {
                capitalizeNext = true;
            }
            else
            {
                result += capitalizeNext
                    ? static_cast<char>(
                        std::toupper(static_cast<unsigned char>(c)))
                    : c;
                capitalizeNext = false;
            }
        }

        return result;
    }

    static auto makeIdentifier(fs::path filePath)
    {
        filePath = filePath.filename();
        return toCamelCase(filePath.string());
    }

public:
    void operator()(fs::path inputFile)
    {
        const auto typeName = StringifyFiles::makeIdentifier(inputFile);

        try
        {
            const auto inputFileString = inputFile.c_str();

            auto inFile = std::ifstream();
            inFile.exceptions(std::ifstream::badbit);
            inFile.open(inputFileString);
            
            auto outFile = std::ofstream();
            outFile.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            outFile.open(
                (this->outputDirectory / inputFile.filename()).string() + ".h");

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

            static constexpr auto maxBatchSizeBytes = size_t{ 50000 };
            static auto buffer =
            [&]
            {
                static constexpr auto bufferSizeBytes =
                    size_t{ 8 * 1024 };

                static constexpr auto maxLengthStringLiteral =
                    size_t{ 65536 };

                static_assert(
                    bufferSizeBytes + maxBatchSizeBytes
                        <= maxLengthStringLiteral);

                return std::array<char, bufferSizeBytes>{};
            }();

            auto batchSizeBytes = size_t{};
            auto batchReadAndWrite =
            [&]()
            {
                inFile.read(buffer.data(), buffer.size());

                const auto bytesRead = inFile.gcount();
                if (bytesRead <= 0)
                {
                    return false;
                }

                outFile.write(buffer.data(), bytesRead);
                if (outFile.bad())
                {
                    throw
                        std::runtime_error(
                            "Error while writing to the file.");
                }

                const auto sizeTBytesRead = static_cast<size_t>(bytesRead);
                if (batchSizeBytes + sizeTBytesRead > maxBatchSizeBytes)
                {
                    outFile
                        << ")DELIMITER\",\n"
                        << "R\"DELIMITER(";

                    batchSizeBytes = 0;
                }

                if (!inFile.eof() && !inFile.good())
                {
                    throw std::runtime_error("Error while reading the file.");
                }

                batchSizeBytes += sizeTBytesRead;

                return true;
            };
            while (batchReadAndWrite());

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
                        "Input file:\n\t{}\n"
                        "Exception details:\n\t{}",
                        inputFile.string().data(),
                        e.what()));
        }
    }

private:
    fs::path outputDirectory;
};
} // namespace ctoAssetsRTIS

int main()
{
    using namespace ctoAssetsRTIS;

    try
    {
        auto stringifyFiles =
            StringifyFiles({
                .outputDirectory =
                []
                {
                    auto argumentBuffer =
                        ArgumentBuffer<"OUTPUT_DIRECTORY"_ads>{};

                    argumentBuffer.readLine();

                    return fs::path(argumentBuffer.getValueView());
                }()
            });

        auto inputFileBuffer = ArgumentBuffer<"INPUT_FILE"_ads>{};
        while (inputFileBuffer.readLine())
        {
            stringifyFiles({ inputFileBuffer.getValueView() });

            inputFileBuffer.forward();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}