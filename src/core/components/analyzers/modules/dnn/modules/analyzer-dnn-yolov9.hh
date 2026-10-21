#pragma once

#include "core/components/analyzers/modules/dnn/analyzer-dnn.hh"
#include "yaml-cpp/node/node.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <string>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            namespace dnn
            {
                class AnalyzerDNNYoloV9 : public AnalyzerDNN
                {
                    public:
                        AnalyzerDNNYoloV9(YAML::Node config, std::string name);
                        AnalyzerDNNYoloV9(const AnalyzerDNNYoloV9& from);
                        ~AnalyzerDNNYoloV9() = default;

                        std::map<std::string, utils::io_data::IODataType> getInputs();
                        std::map<std::string, utils::io_data::IODataType> getOutputs();

                        std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>, source::Source&, bool& trigger);

                        std::unique_ptr<Analyzer> clone();
                    private:
                        struct Detection {
                            int class_id;
                            float confidence;
                            cv::Rect box;
                        };

                        void processDetection(cv::Mat& image, std::vector<Detection> detections, bool& detected);

                        static cv::Mat letterbox(const cv::Mat& src, int targetW, int targetH, float& scale);

                        std::vector<std::string> searching_category;

                        float min_confidence;
                        float nms_threshold;
                };
            }
        }
    }
}
