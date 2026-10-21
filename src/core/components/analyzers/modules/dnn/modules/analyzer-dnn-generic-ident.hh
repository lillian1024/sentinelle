#pragma once

#include "core/components/analyzers/modules/dnn/analyzer-dnn.hh"
#include "yaml-cpp/node/node.h"
#include <string>

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            namespace dnn
            {
                /**
                 * \brief This DNN is specialized in whole-frame object detection.
                 * \details This DNN is lightweight but the objects need to be big on the image to be detected.
                 * \details Link to the DNN: https://huggingface.co/Kalray/ssd-mobilenet-v2
                 * \param image INPUT: The input image
                 * \param debug_image OUTPUT: The image with squares around detected object, class names and confidence percent on the object
                 * \param detected OUPUT: If at least one object has been detected
                 */
                class AnalyzerDNNGenericIdent : public AnalyzerDNN
                {
                    public:
                        AnalyzerDNNGenericIdent(YAML::Node config, std::string name);
                        AnalyzerDNNGenericIdent(const AnalyzerDNNGenericIdent& from);
                        ~AnalyzerDNNGenericIdent() = default;

                        std::map<std::string, utils::io_data::IODataType> getInputs();
                        std::map<std::string, utils::io_data::IODataType> getOutputs();

                        std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>, source::Source&, bool& trigger);

                        std::unique_ptr<Analyzer> clone();
                    private:
                        std::vector<std::string> searching_category;

                        float min_confidence;
                };
            }
        }
    }
}
