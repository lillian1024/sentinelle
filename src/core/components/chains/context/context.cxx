#include "context.hh"
#include "utils/io_data/types/io_data_mat.hh"
#include "utils/io_data/types/io_data_string.hh"
#include <map>
#include <memory>
#include <utility>

#define INPUT_IMAGE_NAME "image"
#define INPUT_SOURCE_NAME "source_name"

namespace core
{
    namespace components
    {
        namespace chains
        {
            const std::string Context::INPUT_STAGE_NAME = "input";
            const int Context::INPUT_STAGE_INDEX = -1;

            Context::Context(cv::Mat& input_image, std::string source_name)
                : data()
            {
                data.insert({INPUT_STAGE_INDEX, std::map<std::string, std::map<std::string, std::unique_ptr<utils::io_data::IOData>>>()});

                auto input_stage = data.find(INPUT_STAGE_INDEX);

                input_stage->second.insert({INPUT_STAGE_NAME, std::map<std::string, std::unique_ptr<utils::io_data::IOData>>()});

                auto input_stage_data = input_stage->second.find(INPUT_STAGE_NAME);

                input_stage_data->second.insert({INPUT_IMAGE_NAME, std::make_unique<utils::io_data::IODataMat>(input_image)});
                input_stage_data->second.insert({INPUT_SOURCE_NAME, std::make_unique<utils::io_data::IODataString>(source_name)});
            }

            bool Context::hasVariable(int stage, std::string analizer_name, std::string output_name) const
            {
                auto stage_data = data.find(stage);

                if (stage_data == data.end())
                {
                    return false;
                }

                auto analizer_data = stage_data->second.find(analizer_name);

                if (analizer_data == stage_data->second.end())
                {
                    return false;
                }

                auto output = analizer_data->second.find(output_name);

                return output != analizer_data->second.end();
            }

            utils::io_data::IOData* Context::getVariable(int stage, std::string analizer_name, std::string output_name) const
            {
                auto stage_data = data.find(stage);

                if (stage_data == data.end())
                {
                    return nullptr;
                }

                auto analizer_data = stage_data->second.find(analizer_name);

                if (analizer_data == stage_data->second.end())
                {
                    return nullptr;
                }

                auto output = analizer_data->second.find(output_name);

                if (output == analizer_data->second.end())
                {
                    return nullptr;
                }

                return output->second.get();
            }

            void Context::setVariable(int stage, std::string analizer_name, std::string output_name, std::unique_ptr<utils::io_data::IOData> value)
            {
                auto stage_data = data.find(stage);

                if (stage_data == data.end())
                {
                    data.insert({stage, std::map<std::string, std::map<std::string, std::unique_ptr<utils::io_data::IOData>>>()});

                    stage_data = data.find(stage);
                }

                auto analizer_data = stage_data->second.find(analizer_name);

                if (analizer_data == stage_data->second.end())
                {
                    stage_data->second.insert({analizer_name, std::map<std::string, std::unique_ptr<utils::io_data::IOData>>()});

                    analizer_data = stage_data->second.find(analizer_name);
                }

                analizer_data->second.insert_or_assign(output_name, std::move(value));
            }

            void Context::storeTemporaryValue(std::unique_ptr<utils::io_data::IOData> value)
            {
                temporary_values.push_back(std::move(value));
            }
        }
    }
}
