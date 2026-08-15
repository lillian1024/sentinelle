#pragma once

#include "core/components/analizers/modules/dnn/analizer-dnn.hh"
#include "yaml-cpp/node/node.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <string>

namespace core
{
    namespace components
    {
        namespace analizer
        {
            namespace dnn
            {
                class AnalizerDNNYoloV9 : public AnalizerDNN
                {
                    public:
                        AnalizerDNNYoloV9(YAML::Node config, std::string name);
                        AnalizerDNNYoloV9(const AnalizerDNNYoloV9& from);
                        ~AnalizerDNNYoloV9() = default;

                        std::map<std::string, utils::io_data::IODataType> getInputs();
                        std::map<std::string, utils::io_data::IODataType> getOutputs();

                        std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>, source::Source&, bool& trigger);

                        std::unique_ptr<Analizer> clone();
                    private:
                        struct Detection {
                            int class_id;
                            float confidence;
                            cv::Rect box;
                        };

                        void processDetection(cv::Mat& image, std::vector<Detection> detections, bool& detected);

                        static cv::Mat letterbox(const cv::Mat& src, int targetW, int targetH, float& scale);

                        std::vector<std::string> searching_category;
                };
            }
        }
    }
}
