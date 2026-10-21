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
             * \brief This analyzer records videos from provided frames.
             * \param image INPUT: The input image
             * \param file INPUT: The name of the file to save
             * \param enable INPUT: If the recorder should save this frame of the video
             */
            class AnalyzerSaveVideo : public Analyzer
            {
                public:
                    AnalyzerSaveVideo(YAML::Node node, std::string name);
                    AnalyzerSaveVideo(const AnalyzerSaveVideo& copy_from);
                    ~AnalyzerSaveVideo();

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    void openNewFile(std::string file, double fps, int frame_width, int frame_height);

                    std::unique_ptr<Analyzer> clone();
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
