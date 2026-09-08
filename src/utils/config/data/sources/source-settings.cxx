#include "source-settings.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data-module.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
#include "core/components/chains/chain.hh"
#include "yaml-cpp/node/node.h"
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#define TRUE_STR "true"

#define IS_LIVE_FIELD "is_live"
#define ENABLED_FIELD "enabled"
#define ACTIVE_FPS_FIELD "active_fps"
#define PASSIVE_FPS_FIELD "passive_fps"
#define PROCESS_FIELD "process"

#define ENABLED_DEFAULT_VALUE TRUE_STR
#define IS_LIVE_DEFAULT_VALUE true

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace sources
            {
                SourceSettings::SourceSettings(YAML::Node node, std::string source_name)
                    : source_name(source_name)
                {
                    std::optional<std::string> is_live_str = DataModule::readScalarOptional(node, IS_LIVE_FIELD);
                    enabled = DataModule::readScalarOptional(node, ENABLED_FIELD).value_or(ENABLED_DEFAULT_VALUE) == TRUE_STR;
                    std::string active_fps_str = DataModule::readScalarOrError(node, ACTIVE_FPS_FIELD, source_name + " source");
                    std::string passive_fps_str = DataModule::readScalarOrError(node, PASSIVE_FPS_FIELD, source_name + " source");
                    YAML::Node process_seq = DataModule::readSequenceOrError(node, PROCESS_FIELD, source_name + " source");

                    if (is_live_str.has_value())
                    {
                        is_live = is_live_str == "true";
                    }
                    else
                    {
                        is_live = IS_LIVE_DEFAULT_VALUE;
                    }

                    try
                    {
                        active_fps = std::stof(active_fps_str);
                        passive_fps = std::stof(passive_fps_str);
                    }
                    catch (const std::invalid_argument&)
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: expected float for " + ACTIVE_FPS_FIELD + " and " + PASSIVE_FPS_FIELD + " field!");
                    }
                    catch (const std::out_of_range&)
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: " + ACTIVE_FPS_FIELD + " or " + PASSIVE_FPS_FIELD + " exceeds max value!");
                    }

                    if (active_fps < 0.0)
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: " + ACTIVE_FPS_FIELD + " must be positive!");
                    }

                    if (passive_fps < 0.0)
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: " + PASSIVE_FPS_FIELD + " must be positive!");
                    }

                    for (size_t i = 0; i < process_seq.size(); i++)
                    {
                        if (!process_seq[i].IsScalar())
                        {
                            throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: All entries in " + PROCESS_FIELD + " must be a string!");
                        }

                        std::string chain_name = process_seq[i].Scalar();

                        if (!ConfigManager::instance().getGeneralSettings().getChainsSettings().hasChain(chain_name))
                        {
                            throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: Unkown chain in " + PROCESS_FIELD + ": " + chain_name + " !");
                        }

                        auto chain = std::make_unique<core::components::chains::Chain>(ConfigManager::instance().getGeneralSettings().getChainsSettings().getChain(chain_name));

                        process.push_back(std::move(chain));
                    }
                }

                SourceSettings::SourceSettings(const SourceSettings& copy_from)
                    : source_name(copy_from.source_name),
                    is_live(copy_from.is_live),
                    active_fps(copy_from.active_fps),
                    passive_fps(copy_from.passive_fps)
                {
                    process = std::vector<std::unique_ptr<core::components::chains::Chain>>();

                    for (size_t i = 0; i < copy_from.process.size(); i++)
                    {
                        auto proc = copy_from.process[i].get();

                        process.push_back(std::make_unique<core::components::chains::Chain>(*proc));
                    }
                }

                SourceSettings::~SourceSettings() { }

                std::unique_ptr<SourceSettings> SourceSettings::getSourceSettingsFromNode(YAML::Node node, std::string source_name)
                {
                    std::string type_name = DataModule::readScalarOrError(node, SOURCE_TYPE_FIELD, source_name + " source");

                    if (type_name == UrlSourceSettings::URL_SOURE_TYPE_NAME)
                    {
                        return std::make_unique<UrlSourceSettings>(node, source_name);
                    }
                    else
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unrecognized source type: " + type_name);
                    }
                }

                std::vector<core::components::chains::Chain*> SourceSettings::getProcess() const
                {
                    std::vector<core::components::chains::Chain*> res;

                    for (size_t i = 0; i < process.size(); i++)
                    {
                        auto proc = process.at(i).get();

                        res.push_back(proc);
                    }

                    return res;
                };
            }
        }
    }
}
