#include "source-settings.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data-module.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#define IS_LIVE_FIELD "is_live"
#define ACTIVE_FPS_FIELD "active_fps"
#define PASSIVE_FPS_FIELD "passive_fps"

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
                    std::string active_fps_str = DataModule::readScalarOrError(node, ACTIVE_FPS_FIELD, source_name + " source");
                    std::string passive_fps_str = DataModule::readScalarOrError(node, PASSIVE_FPS_FIELD, source_name + " source");
                    auto show_debug_view_opt = DataModule::readScalarOptional(node, SHOW_DEBUG_VIEW_FIELD);

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
                    catch (const std::invalid_argument)
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: expected float for " + ACTIVE_FPS_FIELD + " and " + PASSIVE_FPS_FIELD + " field!");
                    }
                    catch (const std::out_of_range)
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

                    if (show_debug_view_opt.has_value() && show_debug_view_opt.value() == "true")
                    {
                        show_debug_view = true;
                    }
                    else
                    {
                        show_debug_view = false;
                    }
                }

                std::unique_ptr<SourceSettings> SourceSettings::getSourceSettingsFromNode(YAML::Node node, std::string source_name)
                {
                    std::string type_name = DataModule::readScalarOrError(node, SOURCE_TYPE_FIELD, "source settings");

                    if (type_name == UrlSourceSettings::URL_SOURE_TYPE_NAME)
                    {
                        return std::make_unique<UrlSourceSettings>(node, source_name);
                    }
                    else
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unrecognized source type: " + type_name);
                    }
                }
            }
        }
    }
}
