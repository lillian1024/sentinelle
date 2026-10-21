#include "analyzer.hh"

#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include "core/components/analyzers/modules/dnn/modules/analyzer-dnn-yolov9.hh"
#include "core/components/analyzers/modules/utils/analyzer-bool-delay.hh"
#include "core/components/analyzers/modules/utils/analyzer-recorder.hh"
#include "core/components/analyzers/modules/utils/analyzer-save-video.hh"
#include "core/components/analyzers/modules/utils/analyzer-show-image.hh"
#include "core/components/analyzers/modules/utils/analyzer-stream-video.hh"
#include "core/components/analyzers/modules/utils/analyzer-trigger-if-true.hh"
#include "modules/dnn/modules/analyzer-dnn-generic-ident.hh"
#include "utils/config/data-module.hh"
#include "utils/io_data/io_data.hh"
#include "utils/logger/logger.hh"

#define GENERIC_ANALIZER_CATEGORY_NAME "AnalyzerDNNGenericIdent"
#define PARENT_FIELD_NAME "analyzers"

#define ANALIZER_TYPE_FIELD "type"

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            const std::string Analyzer::ANALIZE_CATEGORY_NAME = "Analyzer";

            std::unique_ptr<Analyzer>
            Analyzer::getAnalyzerFromNode(YAML::Node node, std::string name)
            {
                std::string type = utils::config::DataModule::readScalarOrError(
                    node, ANALIZER_TYPE_FIELD, PARENT_FIELD_NAME);

                static const std::map<std::string, Type> type_map = {
                    { "dnn_general_ident", Type::DDN_GENERAL_IDENT },
                    { "yolov9-m", Type::YOLOV9M },
                    { "yolov9-p", Type::YOLOV9P },
                    { "show_image", Type::SHOW_IMAGE },
                    { "save_video", Type::SAVE_VIDEO },
                    { "video_recorder", Type::VIDEO_RECORDER },
                    { "stream_video", Type::STREAM_VIDEO },
                    { "trigger_if_true", Type::TRIGGER_IF_TRUE },
                    { "bool_delay", Type::BOOL_DELAY },
                };

                if (type_map.find(type) == type_map.end())
                {
                    throw std::runtime_error(
                        ANALIZE_CATEGORY_NAME
                        + ": Unable to load analyzer: unknown type!");
                }

                Type analyzer_type = type_map.find(type)->second;

                switch (analyzer_type)
                {
                case Type::DDN_GENERAL_IDENT:
                    return std::make_unique<dnn::AnalyzerDNNGenericIdent>(node,
                                                                          name);
                case Type::YOLOV9M:
                    return std::make_unique<dnn::AnalyzerDNNYoloV9>(node, name);
                case Type::YOLOV9P:
                    return std::make_unique<dnn::AnalyzerDNNYoloV9>(node, name);
                case Type::SHOW_IMAGE:
                    return std::make_unique<AnalyzerShowImage>(name);
                case Type::SAVE_VIDEO:
                    return std::make_unique<AnalyzerSaveVideo>(node, name);
                case Type::VIDEO_RECORDER:
                    return std::make_unique<AnalyzerRecorder>(node, name);
                case Type::STREAM_VIDEO:
                    return std::make_unique<AnalyzerStreamVideo>(node, name);
                case Type::TRIGGER_IF_TRUE:
                    return std::make_unique<AnalyzerTriggerIfTrue>(name);
                case Type::BOOL_DELAY:
                    return std::make_unique<AnalyzerBoolDelay>(node, name);
                }

                throw std::runtime_error(
                    ANALIZE_CATEGORY_NAME
                    + ": Unable to load analyzer: unknown type!");
            }

            Analyzer::Analyzer(std::string name)
                : name(name)
            {}

            bool Analyzer::validateInputs(
                std::map<std::string, utils::io_data::IOData*> inputs)
            {
                auto inputTypes = getInputs();

                if (inputTypes.size() != inputs.size())
                {
                    utils::logger::Logger::instance().Log(
                        ANALIZE_CATEGORY_NAME,
                        "Unable to process analyzer: missmatched number of "
                        "arguments!",
                        utils::logger::Logger::LogLevel::ERROR);

                    return false;
                }

                for (auto pair : inputTypes)
                {
                    if (pair.second != inputs[pair.first]->getType())
                    {
                        utils::logger::Logger::instance().Log(
                            ANALIZE_CATEGORY_NAME,
                            "Unable to process analyzer: missmatched argument "
                            "types!",
                            utils::logger::Logger::LogLevel::ERROR);

                        return false;
                    }
                }

                return true;
            }

            bool Analyzer::validateInputs(
                std::map<std::string, utils::io_data::IODataType> types)
            {
                auto inputTypes = getInputs();

                if (inputTypes.size() != types.size())
                {
                    utils::logger::Logger::instance().Log(
                        ANALIZE_CATEGORY_NAME,
                        "Unable to process analyzer: missmatched number of "
                        "arguments!",
                        utils::logger::Logger::LogLevel::ERROR);

                    return false;
                }

                for (auto pair : inputTypes)
                {
                    if (pair.second != types[pair.first])
                    {
                        utils::logger::Logger::instance().Log(
                            ANALIZE_CATEGORY_NAME,
                            "Unable to process analyzer: missmatched argument "
                            "types!",
                            utils::logger::Logger::LogLevel::ERROR);

                        return false;
                    }
                }

                return true;
            }
        } // namespace analyzer
    } // namespace components
} // namespace core
