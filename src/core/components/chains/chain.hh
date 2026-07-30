#pragma once

#include "core/components/analizers/analizer.hh"
#include "core/components/chains/analizer-usage.hh"
#include "core/components/sources/source.hh"
#include "yaml-cpp/node/node.h"
#include <map>
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

                    void process(cv::Mat& input_image, source::Source& source, bool& trigger);

                    std::string dumpConfig();
                private:
                    std::map<std::string, AnalizerUsage> parseStage(YAML::Node node, std::map<std::string, analizer::Analizer&> analizers);
                    std::map<std::string, std::string> parseAnalizerinputs(YAML::Node node);

                    std::vector<std::map<std::string, AnalizerUsage>> stages;
            };
        }
    }
}
