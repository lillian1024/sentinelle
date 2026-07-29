#pragma once

#include "core/components/analizers/analizer.hh"
#include "core/components/chains/context/context.hh"
#include "core/components/chains/path-data.hh"
#include "utils/io_data/io_data.hh"
#include <memory>
#include <optional>
#include <string>
namespace core
{
    namespace components
    {
        namespace chains
        {
            class AnalizerUsage
            {
                public:
                    AnalizerUsage(analizer::Analizer& analizer, std::map<std::string, std::string> inputPath);

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(Context& context, source::Source& source);
                private:
                    static utils::io_data::IOData* getValueFromPath(Context& context, std::string path);

                    static std::optional<bool> tryParseBool(const std::string& value);
                    static std::optional<int> tryParseInt(const std::string& value);
                    static std::optional<float> tryParseFloat(const std::string& value);

                    static bool isAcceptedPath(std::string& path);
                    static bool isValidCharForPath(char c);

                    static std::optional<PathData> getPathData(std::string path);

                    static std::vector<std::string> split(const std::string& input, char delimiter);

                    analizer::Analizer& analizer;
                    std::map<std::string, std::string> inputPath;
            };
        }
    }
}
