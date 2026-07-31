#include "analizer-save-video.hh"
#include "core/components/analizers/analizer.hh"
#include "utils/config/data-module.hh"
#include "utils/io_data/io_data.hh"
#include "utils/io_data/types/io_data_bool.hh"
#include "utils/io_data/types/io_data_mat.hh"
#include "utils/io_data/types/io_data_string.hh"
#include "utils/logger/logger.hh"
#include <iostream>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/videoio.hpp>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#define CATEGORY_NAME "AnalizerSaveVideo"

#define EXTENSION_FIELD_NAME "format"
#define RELEASE_FIELD_NAME "stop_release"

#define IMAGE_INPUT_NAME "image"
#define FILE_NAME_INPUT_NAME "file"
#define ENABLE_INPUT_NAME "enable"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            AnalizerSaveVideo::AnalizerSaveVideo(YAML::Node node, std::string name)
                : Analizer(name)
            {
                std::string format_str = utils::config::DataModule::readScalarOrError(node, EXTENSION_FIELD_NAME, CATEGORY_NAME);
                stop_release = utils::config::DataModule::readScalarOrError(node, RELEASE_FIELD_NAME, CATEGORY_NAME) == "true";
                previous_file = "";

                format = getFourcc(format_str);

                writer = std::nullopt;
            }

            AnalizerSaveVideo::AnalizerSaveVideo(const AnalizerSaveVideo& copy_from)
                : Analizer(copy_from.name)
            {
                format = copy_from.format;
                stop_release = copy_from.stop_release;
                previous_file = "";
                writer = std::nullopt;
            }

            AnalizerSaveVideo::~AnalizerSaveVideo()
            {
                if (writer != std::nullopt)
                {
                    writer->release();
                }
            }

            std::map<std::string, utils::io_data::IODataType> AnalizerSaveVideo::getInputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                res.insert({IMAGE_INPUT_NAME, utils::io_data::IODataType::MAT});
                res.insert({FILE_NAME_INPUT_NAME, utils::io_data::IODataType::STRING});
                res.insert({ENABLE_INPUT_NAME, utils::io_data::IODataType::BOOL});

                return res;
            }
            std::map<std::string, utils::io_data::IODataType> AnalizerSaveVideo::getOutputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                return res;
            }

            std::map<std::string, std::unique_ptr<utils::io_data::IOData>> AnalizerSaveVideo::process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& _)
            {
                auto image_data_field = inputs.find(IMAGE_INPUT_NAME);
                auto file_data_field = inputs.find(FILE_NAME_INPUT_NAME);
                auto enable_data_field = inputs.find(ENABLE_INPUT_NAME);

                // Check that input exist
                if (image_data_field == inputs.end() || file_data_field == inputs.end() || enable_data_field == inputs.end())
                {
                    utils::logger::Logger::instance().Log(CATEGORY_NAME, "Mismatched arguments!", utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                auto* image_data = dynamic_cast<utils::io_data::IODataMat*>(image_data_field->second);
                auto* file_data = dynamic_cast<utils::io_data::IODataString*>(file_data_field->second);
                auto* enable_data = dynamic_cast<utils::io_data::IODataBool*>(enable_data_field->second);

                // Check that input is the right type
                if (image_data == nullptr || file_data == nullptr || enable_data == nullptr)
                {
                    std::ostringstream sb;

                    sb << "Mismatched argument type: ";
                    if (image_data == nullptr)
                    {
                        sb << IMAGE_INPUT_NAME;
                        sb << ' ';
                    }
                    if (file_data == nullptr)
                    {
                        sb << FILE_NAME_INPUT_NAME;
                        sb << ' ';
                    }
                    if (enable_data == nullptr)
                    {
                        sb << ENABLE_INPUT_NAME;
                        sb << ' ';
                    }
                    sb << '!';

                    utils::logger::Logger::instance().Log(CATEGORY_NAME, sb.str(), utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                cv::Mat input_image = image_data->getData();
                std::string input_file = file_data->getData();
                bool enable = enable_data->getData();

                // Check that file name is valid
                if (input_file.empty())
                {
                    utils::logger::Logger::instance().Log(CATEGORY_NAME, "Unable to save to file: '" + input_file + "'!", utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                if (!enable)
                {
                    if (writer.has_value() && stop_release)
                    {
                        writer->release();
                        writer = std::nullopt;
                    }

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                int frame_width = input_image.cols;
                int frame_height = input_image.rows;

                // If file not open or changing file
                if (input_file != previous_file || !writer.has_value())
                {
                    // TODO: change fps to current fps with futur is_active value
                    openNewFile(input_file, source.getFPS(true), frame_width, frame_height);
                }

                // Check that writer has been initialized correctly
                if (!writer.has_value() || !writer->isOpened())
                {
                    utils::logger::Logger::instance().Log(CATEGORY_NAME, "Unable to save to file: '" + input_file + "'!", utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                // Write the frame
                writer->write(input_image);

                return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
            }

            void AnalizerSaveVideo::openNewFile(std::string file, double fps, int frame_width, int frame_height)
            {
                if (writer.has_value())
                {
                    writer->release();

                    std::cout << "Released writer!3" << std::endl;

                    writer = std::nullopt;
                }

                cv::Size frame_size(frame_width, frame_height);

                writer = cv::VideoWriter(file, format, fps, frame_size);

                previous_file = file;
            }

            std::unique_ptr<Analizer> AnalizerSaveVideo::clone()
            {
                return std::make_unique<AnalizerSaveVideo>(*this);
            }

            int AnalizerSaveVideo::getFourcc(std::string format)
            {
                //std::transform(format.begin(), format.end(), format.begin(), ::toupper);

                if (format.length() != 4)
                {
                    std::ostringstream sb;

                    sb << "[";
                    sb << CATEGORY_NAME;
                    sb << "]: Unable to parse format: '";
                    sb << format;
                    sb << "': unrecognized format!";

                    throw std::runtime_error(sb.str());
                }

                char c1 = format[0];
                char c2 = format[1];
                char c3 = format[2];
                char c4 = format[3];

                return cv::VideoWriter::fourcc(c1,c2,c3,c4);
            }
        }
    }
}
