#include "analizer.hh"

#include "core/components/analizers/modules/utils/analizer-show-image.hh"
#include "core/components/analizers/modules/utils/analizer-save-video.hh"
#include "modules/dnn/modules/analizer-dnn-generic-ident.hh"
#include "utils/config/data-module.hh"
#include "utils/io_data/io_data.hh"
#include "utils/logger/logger.hh"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#define GENERIC_ANALIZER_CATEGORY_NAME "AnalizerDNNGenericIdent"
#define PARENT_FIELD_NAME "analizers"

#define ANALIZER_TYPE_FIELD "type"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            const std::string Analizer::ANALIZE_CATEGORY_NAME = "Analizer";

            std::unique_ptr<Analizer> Analizer::getAnalizerFromNode(YAML::Node node, std::string name)
            {
                std::string type = utils::config::DataModule::readScalarOrError(node, ANALIZER_TYPE_FIELD, PARENT_FIELD_NAME);

                static const std::map<std::string, Type> type_map = {
                    {"dnn_general_ident", Type::DDN_GENERAL_IDENT},
                    {"show_image", Type::SHOW_IMAGE},
                    {"save_video", Type::SAVE_VIDEO},
                };

                if (type_map.find(type) == type_map.end())
                {
                    throw std::runtime_error(ANALIZE_CATEGORY_NAME + ": Unable to load analizer: unknown type!");
                }

                Type analizer_type = type_map.find(type)->second;

                switch (analizer_type)
                {
                    case Type::DDN_GENERAL_IDENT:
                        return std::make_unique<dnn::AnalizerDNNGenericIdent>(node, name);
                    case Type::SHOW_IMAGE:
                        return std::make_unique<AnalizerShowImage>(name);
                    case Type::SAVE_VIDEO:
                        return std::make_unique<AnalizerSaveVideo>(node, name);
                }

                throw std::runtime_error(ANALIZE_CATEGORY_NAME + ": Unable to load analizer: unknown type!");
            }

            Analizer::Analizer(std::string name)
                : name(name) {}

            bool Analizer::validateInputs(std::map<std::string, utils::io_data::IOData*> inputs)
            {
                auto inputTypes = getInputs();

                if (inputTypes.size() != inputs.size())
                {
                    utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to process analizer: missmatched number of arguments!", utils::logger::Logger::LogLevel::ERROR);

                    return false;
                }

                for (auto pair: inputTypes)
                {
                    if (pair.second != inputs[pair.first]->getType())
                    {
                        utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to process analizer: missmatched argument types!", utils::logger::Logger::LogLevel::ERROR);

                        return false;
                    }
                }

                return true;
            }

            bool Analizer::validateInputs(std::map<std::string, utils::io_data::IODataType> types)
            {
                auto inputTypes = getInputs();

                if (inputTypes.size() != types.size())
                {
                    utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to process analizer: missmatched number of arguments!", utils::logger::Logger::LogLevel::ERROR);

                    return false;
                }

                for (auto pair: inputTypes)
                {
                    if (pair.second != types[pair.first])
                    {
                        utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to process analizer: missmatched argument types!", utils::logger::Logger::LogLevel::ERROR);

                        return false;
                    }
                }

                return true;
            }
        }
    }
}
