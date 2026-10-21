#pragma once

#include "core/components/analyzers/analyzer.hh"

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            class AnalyzerShowImage : public Analyzer
            {
                public:
                    AnalyzerShowImage(std::string name);
                    ~AnalyzerShowImage() = default;

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    std::unique_ptr<Analyzer> clone();
                private:
                    typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> time_type;

                    time_type prev_frame;
            };
        }
    }
}
