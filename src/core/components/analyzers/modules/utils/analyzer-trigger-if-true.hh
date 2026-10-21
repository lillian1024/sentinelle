#pragma once

#include "core/components/analyzers/analyzer.hh"
#include "utils/io_data/io_data.hh"

#include <map>
#include <string>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            /**
             * \brief This analyzer triggers the current source if the input value is true. It does nothing otherwise.
             * \param input If the source should be triggered
             */
            class AnalyzerTriggerIfTrue : public Analyzer
            {
                public:
                    AnalyzerTriggerIfTrue(std::string name);
                    ~AnalyzerTriggerIfTrue() = default;

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    std::unique_ptr<Analyzer> clone();
            };
        }
    }
}
