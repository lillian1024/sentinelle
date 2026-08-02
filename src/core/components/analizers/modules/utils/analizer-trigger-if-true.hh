#pragma once

#include "core/components/analizers/analizer.hh"
#include "utils/io_data/io_data.hh"

#include <map>
#include <string>

namespace core
{
    namespace components
    {
        namespace analizer
        {
            class AnalizerTriggerIfTrue : public Analizer
            {
                public:
                    AnalizerTriggerIfTrue(std::string name);
                    ~AnalizerTriggerIfTrue() = default;

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    std::unique_ptr<Analizer> clone();
            };
        }
    }
}
