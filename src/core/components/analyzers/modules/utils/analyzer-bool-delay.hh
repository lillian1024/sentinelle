#pragma once

#include "core/components/analyzers/analyzer.hh"

#include <cstddef>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            /**
             * \brief This analyzer delays the rising or falling edge of a boolean value.
             * \details The delay is set as a number of iteration of this analyzer specified in the configuration.
             * \details Examples:
             * \details - If a rising edge value is delayed by 5 iterations. The input value will need to be `true` for at least 5 consecutive iterations of the process() function to have the output switch from `false` to `true`
             * \details - If a falling edge value is delayed by 5 iterations. The input value will need to be `false` for at least 5 consecutive iterations of the process() function to have the output switch from `true` to `false`
             * \param input INPUT: The boolean input value
             * \param output OUTPUT: The boolean output value
             */
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
