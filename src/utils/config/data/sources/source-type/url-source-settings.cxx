#include "url-source-settings.hh"
#include "utils/config/data-module.hh"
#include "utils/config/data/sources/source-settings.hh"
#include <stdexcept>
#include <string>

#define PARENT_FIELD_NAME "source settings"

#define URL_FIELD "url"
#define ACTIVE_FPS_FIELD "active-fps"
#define PASSIVE_FPS_FIELD "passive-fps"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace sources
            {
                const std::string UrlSourceSettings::URL_SOURE_TYPE_NAME = "url";

                UrlSourceSettings::UrlSourceSettings(YAML::Node node)
                {
                    const std::string msgPrefix = "[ConfigManager]: ";

                    if (!node.IsDefined())
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: error while parsing source!");
                    }

                    if (DataModule::readScalarOrError(node, SOURCE_TYPE_FIELD, PARENT_FIELD_NAME) != URL_SOURE_TYPE_NAME)
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: mismatched source type! please report this error to the maintainers.");
                    }

                    url = DataModule::readScalarOrError(node, URL_FIELD, PARENT_FIELD_NAME);
                    std::string active_fps_str = DataModule::readScalarOrError(node, ACTIVE_FPS_FIELD, PARENT_FIELD_NAME);
                    std::string passive_fps_str = DataModule::readScalarOrError(node, PASSIVE_FPS_FIELD, PARENT_FIELD_NAME);

                    try
                    {
                        active_fps = std::stof(active_fps_str);
                        passive_fps = std::stof(passive_fps_str);
                    }
                    catch (const std::invalid_argument)
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: expected float for " + ACTIVE_FPS_FIELD + " and " + PASSIVE_FPS_FIELD + " field!");
                    }
                    catch (const std::out_of_range)
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: " + ACTIVE_FPS_FIELD + " or " + PASSIVE_FPS_FIELD + " exceeds max value!");
                    }

                    if (active_fps < 0.0)
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: " + ACTIVE_FPS_FIELD + " must be positive!");
                    }

                    if (passive_fps < 0.0)
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: " + PASSIVE_FPS_FIELD + " must be positive!");
                    }
                }
            }
        }
    }
}
