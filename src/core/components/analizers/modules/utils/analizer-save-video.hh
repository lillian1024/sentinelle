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
            class AnalizerSaveVideo : public Analizer
            {
                public:
                    AnalizerSaveVideo(YAML::Node node, std::string name);
                    AnalizerSaveVideo(const AnalizerSaveVideo& copy_from);
                    ~AnalizerSaveVideo();

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>, source::Source&, bool& trigger);

                    void openNewFile(std::string file, double fps, int frame_width, int frame_height);

                    std::unique_ptr<Analizer> clone();
                protected:


                    int getFourcc(std::string format);

                    int format;
                    bool stop_release;

                    std::string previous_file;

                    std::optional<cv::VideoWriter> writer;
            };
        }
    }
}
