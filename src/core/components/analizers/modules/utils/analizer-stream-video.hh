#pragma once

#include "core/components/analizers/analizer.hh"
#include "yaml-cpp/node/node.h"
#include <opencv2/videoio.hpp>
#include <optional>
#include <string>

namespace core
{
    namespace components
    {
        namespace analizer
        {
            class AnalizerStreamVideo : public Analizer
            {
                public:
                    AnalizerStreamVideo(YAML::Node node, std::string name);
                    AnalizerStreamVideo(const AnalizerStreamVideo& copy_from);
                    ~AnalizerStreamVideo();

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    void openNewStream(std::string stream_name, double fps, int frame_width, int frame_height);

                    std::unique_ptr<Analizer> clone();
                protected:


                    int getFourcc(std::string format);

                    bool stop_release;

                    std::string stream_name;

                    std::optional<cv::VideoWriter> writer;
            };
        }
    }
}
