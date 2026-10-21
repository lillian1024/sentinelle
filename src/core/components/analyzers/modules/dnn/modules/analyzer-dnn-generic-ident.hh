#pragma once

#include "core/components/analyzers/modules/dnn/analyzer-dnn.hh"
#include "yaml-cpp/node/node.h"
#include <string>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            namespace dnn
            {
                class AnalyzerDNNGenericIdent : public AnalyzerDNN
                {
                    public:
                        AnalyzerDNNGenericIdent(YAML::Node config, std::string name);
                        AnalyzerDNNGenericIdent(const AnalyzerDNNGenericIdent& from);
                        ~AnalyzerDNNGenericIdent() = default;

                        std::map<std::string, utils::io_data::IODataType> getInputs();
                        std::map<std::string, utils::io_data::IODataType> getOutputs();

                        std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>, source::Source&, bool& trigger);

                        std::unique_ptr<Analyzer> clone();
                    private:
                        std::vector<std::string> searching_category;

                        float min_confidence;
                };
            }
        }
    }
}
