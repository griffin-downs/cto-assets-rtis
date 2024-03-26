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


namespace ctoAssetsRTIS
{
template<AutomaticDurationString... ArgumentNames>
class ArgumentBuffers
{
public:
    template<AutomaticDurationString KeyName>
    struct ArgumentBuffer
    {
        static constexpr auto keyName = KeyName.data;
        std::string buffer;
    };

private:
    std::tuple<ArgumentBuffer<ArgumentNames>...> argumentBuffers;

public:
    bool readInput()
    {
        static constexpr auto applyToEachConjunctive =
        [](const auto& operation, auto& tuple)
        {
            const auto apply =
            [&]<size_t... Indices>(std::index_sequence<Indices...>)
            {
                return (operation(std::get<Indices>(tuple)) && ...);
            };

            return apply(
                std::make_index_sequence<
                    std::tuple_size<
                        typename std::decay<decltype(tuple)>::type
                    >::value
                >{});
        };

        static constexpr auto read =
        [](auto& argumentBuffer)
        {
            if (std::getline(std::cin, argumentBuffer.buffer))
            {
                return true;
            }

            if (!std::cin.fail() || std::cin.eof())
            {
                return false;
            }

            throw std::runtime_error(
                std::format(
                    "Error: Failed to read input for parameter: {}",
                    argumentBuffer.keyName));
        };

        return applyToEachConjunctive(read, this->argumentBuffers);
    }

    auto extractValues() const
    {
        static constexpr auto extractValue =
        [](auto& argumentBuffer) -> const char*
        {
            auto& buffer = argumentBuffer.buffer;
            const auto keyName = argumentBuffer.keyName;

            if (!buffer.starts_with(keyName))
            {
                throw std::runtime_error(
                    std::format(
                        "Key '{}' not found in the buffer: {}",
                        keyName,
                        buffer));
            }

            const auto delimiter = "=";

            const auto
                delimiterLength = std::char_traits<char>::length(delimiter),
                keyNameLength = std::char_traits<char>::length(keyName);

            if ([&]
            {
                const auto remainingBuffer = buffer.substr(keyNameLength);
                return !remainingBuffer.starts_with(delimiter);
            }())
            {
                throw std::runtime_error(
                    std::format(
                        "Delimiter '{}' not found in the buffer: {}",
                        delimiter,
                        buffer));
            }

            const auto keyNameDelimiterLength =
                keyNameLength + delimiterLength;

            return buffer.data() + keyNameDelimiterLength;
        };

        const auto transform =
        [&]<size_t... Indices>(std::index_sequence<Indices...>)
        {
            return
                std::make_tuple(
                    extractValue(
                        std::get<Indices>(this->argumentBuffers))...);
        };

        return transform(
            std::make_index_sequence<
                std::tuple_size<decltype(this->argumentBuffers)>::value
            >{});
    }
};

template<typename T>
concept IsConstCharPointer = std::is_same<T, const char*>::value;

template<IsConstCharPointer... ArgumentTypes>
void stringifyFile(std::tuple<ArgumentTypes...> arguments)
{
    auto [inputFilePath, outputFilePath, typeName] = arguments;

    std::ifstream inFile;
    std::ofstream outFile;

    try
    {
        inFile.open(inputFilePath, std::ios::binary);
        inFile.exceptions(std::ifstream::badbit);

        outFile.open(outputFilePath, std::ios::binary);
        outFile.exceptions(std::ofstream::badbit);

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
                    <=
                    maxLengthStringLiteral);

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
        static constexpr auto stringFormatTuple =
        [](const auto& tuple)
        {
            const auto format =
            [&]<size_t... Indices>(std::index_sequence<Indices...>)
            {
                std::ostringstream stream;

                ((stream << "\n\t" << std::get<Indices>(tuple)), ...);

                return stream.str();
            };

            return format(
                std::make_index_sequence<
                    std::tuple_size<
                        typename std::decay<decltype(tuple)>::type
                    >::value
                >{});
        };

        static constexpr auto errorMessageTemplate =
            "Error reading/writing files.\n"
            "Arguments:{}\n"
            "Exception details:\n\t{}";

        throw std::runtime_error(
            std::format(
                errorMessageTemplate,
                stringFormatTuple(arguments),
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

        while (argumentBuffers.readInput())
        {
            stringifyFile(argumentBuffers.extractValues());
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}