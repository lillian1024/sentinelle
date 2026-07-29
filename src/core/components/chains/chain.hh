#pragma once

#include "core/components/analizers/analizer.hh"
#include "core/components/chains/analizer-usage.hh"
#include "core/components/chains/path-data.hh"
#include "yaml-cpp/node/node.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace core
{
    namespace components
    {
        namespace chains
        {
            class Chain
            {
                public:
                    Chain(YAML::Node node, std::map<std::string, analizer::Analizer&> analizers);

                private:
                    std::map<std::string, AnalizerUsage> parseStage(YAML::Node node, std::map<std::string, analizer::Analizer&> analizers);
                    std::map<std::string, PathData> parseAnalizerinputs(YAML::Node node);

                    static bool isAcceptedPath(std::string path);
                    static bool isValidCharForPath(char c);

                    static std::optional<PathData> getPathData(std::string path);

                    static std::vector<std::string> split(std::string input, char delimiter);

                    std::vector<std::map<std::string, AnalizerUsage>> stages;
            };
        }
    }
}
