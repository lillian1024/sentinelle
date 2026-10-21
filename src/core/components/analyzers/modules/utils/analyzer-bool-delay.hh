#pragma once

#include "core/components/analyzers/analyzer.hh"

#include <cstddef>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            class AnalyzerBoolDelay : public Analyzer
            {
                public:
                    AnalyzerBoolDelay(YAML::Node node, std::string name);
                    ~AnalyzerBoolDelay() = default;

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    std::unique_ptr<Analyzer> clone();
                protected:
                    bool start_value;

                    unsigned int charge_delay;
                    unsigned int discharge_delay;

                    long int charge_status;
                    long int discharge_status;
            };
        }
    }
}
