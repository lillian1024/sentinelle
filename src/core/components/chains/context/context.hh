#pragma once

#include "utils/io_data/io_data.hh"
#include <map>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <string>

namespace core
{
    namespace components
    {
        namespace chains
        {
            class Context
            {
                public:
                    Context(cv::Mat input_image, std::string source_name);
                    ~Context() = default;

                    static const std::string INPUT_STAGE_NAME;
                    static const int INPUT_STAGE_INDEX = -1;

                    bool hasVariable(int stage, std::string analizer_name, std::string output_name) const;

                    utils::io_data::IOData* getVariable(int stage, std::string analizer_name, std::string output_name) const;

                    void setVariable(int stage, std::string analizer_name, std::string output_name, std::unique_ptr<utils::io_data::IOData> value);
                private:
                    std::map<int, std::map<std::string, std::map<std::string, std::unique_ptr<utils::io_data::IOData>>>> data;
            };
        }
    }
}
