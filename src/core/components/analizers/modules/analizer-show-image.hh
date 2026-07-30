#pragma once

#include "core/components/analizers/analizer.hh"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            class AnalizerShowImage : public Analizer
            {
                public:
                    AnalizerShowImage(std::string name);
                    ~AnalizerShowImage() = default;

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>, source::Source&, bool& trigger);
                private:
                    typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> time_type;

                    time_type prev_frame;
            };
        }
    }
}
