#pragma once

#include "core/components/analizers/modules/dnn/analizer-dnn.hh"
#include "yaml-cpp/node/node.h"
#include <string>

namespace core
{
    namespace components
    {
        namespace analizer
        {
            namespace dnn
            {
                class AnalizerDNNGenericIdent : public AnalizerDNN
                {
                    public:
                        AnalizerDNNGenericIdent(YAML::Node config, std::string name);
                        ~AnalizerDNNGenericIdent() = default;

                        std::map<std::string, utils::io_data::IODataType> getInputs();
                        std::map<std::string, utils::io_data::IODataType> getOutputs();

                        std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>, source::Source&);
                    private:
                        std::vector<std::string> class_names;
                };
            }
        }
    }
}
