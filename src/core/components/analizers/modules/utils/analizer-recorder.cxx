#include "analizer-recorder.hh"
#include "core/components/analizers/analizer.hh"
#include "utils/config/data-module.hh"
#include "utils/io_data/io_data.hh"
#include "utils/io_data/types/io_data_bool.hh"
#include "utils/io_data/types/io_data_mat.hh"
#include "utils/io_data/types/io_data_string.hh"
#include "utils/logger/logger.hh"
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/videoio.hpp>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#define CATEGORY_NAME "AnalizerRecorder"

#define FORMAT_FIELD_NAME "format"
#define EXTENSION_FIELD_NAME "extension"
#define VIDEO_FPS_FIELD_NAME "fps"
#define COMMENT_FIELD_NAME "comment"
#define STORAGE_PATH_FIELD_NAME "storage_path"
#define RELEASE_FIELD_NAME "stop_release"

#define COMMENT_DEFAULT_VALUE ""

#define IMAGE_INPUT_NAME "image"
#define FILE_PREFIX_INPUT_NAME "prefix"
#define ENABLE_INPUT_NAME "enable"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            unsigned int AnalizerRecorder::AnalizerRecorder::analizer_count = 0;

            AnalizerRecorder::AnalizerRecorder(YAML::Node node, std::string name)
                : Analizer(name)
            {
                analizer_id = getNewId();

                std::string format_str = utils::config::DataModule::readScalarOrError(node, FORMAT_FIELD_NAME, CATEGORY_NAME);
                file_extension = utils::config::DataModule::readScalarOrError(node, EXTENSION_FIELD_NAME, CATEGORY_NAME);
                video_fps = utils::config::DataModule::readDouble(node, VIDEO_FPS_FIELD_NAME, CATEGORY_NAME, false, "not used");
                file_comment = utils::config::DataModule::readScalarOptional(node, COMMENT_FIELD_NAME).value_or(COMMENT_DEFAULT_VALUE);
                stop_release = utils::config::DataModule::readBool(node, RELEASE_FIELD_NAME, CATEGORY_NAME, false, "true");

                storage_path = utils::config::DataModule::readScalarOrError(node, STORAGE_PATH_FIELD_NAME, CATEGORY_NAME);

                format = getFourcc(format_str);

                writer = std::nullopt;
            }

            AnalizerRecorder::AnalizerRecorder(const AnalizerRecorder& copy_from)
                : Analizer(copy_from.name),
                format(copy_from.format),
                file_extension(copy_from.file_extension),
                video_fps(copy_from.video_fps),
                file_comment(copy_from.file_comment),
                storage_path(copy_from.storage_path),
                stop_release(copy_from.stop_release)
            {
                analizer_id = getNewId();

                writer = std::nullopt;
            }

            AnalizerRecorder::~AnalizerRecorder()
            {
                if (writer != std::nullopt)
                {
                    writer->release();
                }
            }

            unsigned int AnalizerRecorder::getNewId()
            {
                unsigned int id = analizer_count;
                analizer_count++;

                return id;
            }

            std::map<std::string, utils::io_data::IODataType> AnalizerRecorder::getInputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                res.insert({IMAGE_INPUT_NAME, utils::io_data::IODataType::MAT});
                res.insert({FILE_PREFIX_INPUT_NAME, utils::io_data::IODataType::STRING});
                res.insert({ENABLE_INPUT_NAME, utils::io_data::IODataType::BOOL});

                return res;
            }
            std::map<std::string, utils::io_data::IODataType> AnalizerRecorder::getOutputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                return res;
            }

            std::map<std::string, std::unique_ptr<utils::io_data::IOData>> AnalizerRecorder::process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source&, bool&)
            {
                auto image_data_field = inputs.find(IMAGE_INPUT_NAME);
                auto file_data_field = inputs.find(FILE_PREFIX_INPUT_NAME);
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
                        sb << FILE_PREFIX_INPUT_NAME;
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
                std::string file_prefix = file_data->getData();
                bool enable = enable_data->getData();

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
                if (!writer.has_value())
                {
                    // TODO: change fps to current fps with futur is_active value
                    openNewFile(file_prefix, frame_width, frame_height);
                }

                // Check that writer has been initialized correctly
                if (!writer.has_value() || !writer->isOpened())
                {
                    utils::logger::Logger::instance().Log(CATEGORY_NAME, "Unable to save to file: '" + ComposeFileName(file_prefix) + "'!", utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                // Write the frame
                writer->write(input_image);

                return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
            }

            void AnalizerRecorder::openNewFile(std::string file_prefix, int frame_width, int frame_height)
            {
                if (writer.has_value())
                {
                    writer->release();

                    utils::logger::Logger::instance().Log(CATEGORY_NAME, "Released previous writer", utils::logger::Logger::LogLevel::DEBUG);

                    writer = std::nullopt;
                }

                cv::Size frame_size(frame_width, frame_height);

                writer = cv::VideoWriter(ComposeFileName(file_prefix), format, video_fps, frame_size);
            }

            std::string AnalizerRecorder::ComposeFileName(std::string file_prefix)
            {
                std::ostringstream sb;

                sb << storage_path;

                if (!storage_path.empty())
                {
                    sb << '/';
                }

                sb << file_prefix;
                if (!file_prefix.empty())
                {
                    sb << '_';
                }

                std::time_t t = std::time(nullptr);

                sb << std::put_time(std::localtime(&t), "%d-%m-%Y_%X");

                if (!file_comment.empty())
                {
                    sb << '_' << file_comment;
                }

                sb << '.' << file_extension;

                return sb.str();
            }

            std::unique_ptr<Analizer> AnalizerRecorder::clone()
            {
                return std::make_unique<AnalizerRecorder>(*this);
            }

            int AnalizerRecorder::getFourcc(std::string format)
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
