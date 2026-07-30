#include "analizer-show-image.hh"
#include "utils/io_data/io_data.hh"
#include "utils/io_data/types/io_data_mat.hh"
#include "utils/io_data/types/io_data_string.hh"
#include "utils/logger/logger.hh"
#include <iostream>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui.hpp>

#define IMAGE_INPUT_FIELD "image"
#define NAME_INPUT_FIELD "name"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            AnalizerShowImage::AnalizerShowImage(std::string name)
                : Analizer(name),
                  prev_frame(std::chrono::steady_clock::now()) { }

            std::map<std::string, utils::io_data::IODataType> AnalizerShowImage::getInputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                res.insert({IMAGE_INPUT_FIELD, utils::io_data::IODataType::MAT});
                res.insert({NAME_INPUT_FIELD, utils::io_data::IODataType::STRING});

                return res;
            }
            std::map<std::string, utils::io_data::IODataType> AnalizerShowImage::getOutputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                return res;
            }

            std::map<std::string, std::unique_ptr<utils::io_data::IOData>> AnalizerShowImage::process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger)
            {
                if (!validateInputs(inputs))
                {
                    utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to analize: missmatched arguments!", utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                // Compute fps
                auto current_time = source.getLastFrameTime();

                float time_interval_sec = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - prev_frame).count();

                prev_frame = current_time;

                float prev_fps;

                if (time_interval_sec == 0.0)
                {
                    prev_fps = 0;
                }
                else
                {
                    prev_fps = 1 / (time_interval_sec);
                }

                // Get inputs
                utils::io_data::IODataMat* image_data = getInputTypeFromInputs<utils::io_data::IODataMat>(inputs, IMAGE_INPUT_FIELD);
                utils::io_data::IODataString* name_data = getInputTypeFromInputs<utils::io_data::IODataString>(inputs, NAME_INPUT_FIELD);

                if (!image_data || !name_data)
                {
                    utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to analize: missmatched arguments!", utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                cv::Mat image = image_data->getData();

                if (image.empty())
                {
                    utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to diplay image: image is empty!", utils::logger::Logger::LogLevel::WARNING);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                // Display image
                std::ostringstream box_string_builder;

                box_string_builder << "FPS: ";
                box_string_builder << prev_fps;

                cv::putText(image, box_string_builder.str(), cv::Point(5, 75), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0,255,255), 1);

                cv::imshow(name_data->getData(), image);

                // This line seems mendatory for the window to show
                // TODO: replace by a better function
                int _ = cv::waitKey(10);

                return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
            }
        }
    }
}
