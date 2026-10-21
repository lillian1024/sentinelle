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
             * \brief This analyzer allows smart recording of videos onto the host's filesystem.
             * \details This analyzer saves videos depending on the input value and set the name of the file to a unique human readable name.
             * \param image INPUT: The input image
             * \param prefix INPUT: The file name's prefix
             * \param enable INPUT: If the recorder should save this frame of the video
             */
            class AnalyzerRecorder : public Analyzer
            {
                public:
                    AnalyzerRecorder(YAML::Node node, std::string name);
                    AnalyzerRecorder(const AnalyzerRecorder& copy_from);
                    ~AnalyzerRecorder();

                    std::map<std::string, utils::io_data::IODataType> getInputs();
                    std::map<std::string, utils::io_data::IODataType> getOutputs();

                    std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger);

                    void openNewFile(std::string file_prefix, int frame_width, int frame_height);

                    std::unique_ptr<Analyzer> clone();
                protected:
                    static unsigned int analyzer_count;

                    static unsigned int getNewId();

                    virtual std::string ComposeFileName(std::string file_comment);

                    int getFourcc(std::string format);

                    int format;
                    std::string file_extension;
                    double video_fps;

                    std::string file_comment;

                    std::string storage_path;

                    bool stop_release;

                    unsigned int analyzer_id;

                    std::optional<cv::VideoWriter> writer;
            };
        }
    }
}
