#pragma once

#include "core/components/analizers/analizer.hh"

#include <cstddef>

namespace core
{
    namespace components
    {
        namespace analizer
        {
            class AnalizerBoolDelay : public Analizer
            {
                public:
                    AnalizerBoolDelay(YAML::Node node, std::string name);
                    ~AnalizerBoolDelay() = default;

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    std::unique_ptr<Analizer> clone();
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
