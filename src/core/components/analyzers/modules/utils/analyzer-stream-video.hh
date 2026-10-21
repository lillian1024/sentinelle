#pragma once

#include "core/components/analyzers/analyzer.hh"
#include "yaml-cpp/node/node.h"
#include <opencv2/videoio.hpp>
#include <optional>
#include <string>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            /**
             * \brief WIP: This analyzer create a video stream of the provided frames.
             * \param image INPUT: The image input
             * \param file INPUT: The stream name
             * \param enable INPUT: If the provided frame should be streamed
             */
            class AnalyzerStreamVideo : public Analyzer
            {
                public:
                    AnalyzerStreamVideo(YAML::Node node, std::string name);
                    AnalyzerStreamVideo(const AnalyzerStreamVideo& copy_from);
                    ~AnalyzerStreamVideo();

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    void openNewStream(std::string stream_name, double fps, int frame_width, int frame_height);

                    std::unique_ptr<Analyzer> clone();
                protected:
                    int getFourcc(std::string format);

                    std::string encoder;
                    int bitrate;
                    bool low_lat;
                    std::string additional_args;

                    bool stop_release;

                    std::string stream_name;

                    std::optional<cv::VideoWriter> writer;
            };
        }
    }
}
