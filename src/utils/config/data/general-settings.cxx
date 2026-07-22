#include "general-settings.hh"

#include "sources/sources-settings.hh"
#include "utils/config/config-manager.hh"
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <string>

#define PARENT_FIELD_NAME "general settings"
#define SERVER_NAME_FIELD "server_name"
#define THREAD_LIMIT_FIELD "thread_limit"

//Volontary underflow of size_t (unsigned) to get size_t's max value
#define THREAD_LIMIT_DEFAULT_VALUE -1

namespace utils
{
    namespace config
    {
        namespace data
        {
            void GeneralSettings::readModule(YAML::Node node)
            {
                server_name = readScalarOrError(node, SERVER_NAME_FIELD, PARENT_FIELD_NAME);
                std::optional<std::string> thread_limit_str = readScalarOptional(node, THREAD_LIMIT_FIELD, PARENT_FIELD_NAME);
                sources_settings.readModule(readMapOrError(node, SOURCES_SETTINGS_FIELD_NAME, PARENT_FIELD_NAME));

                if (thread_limit_str.has_value())
                {
                    try
                    {
                        //Volontary possible size_t (unsigned) underflow to get size_t's max value
                        thread_limit = std::stol(thread_limit_str.value());
                    }
                    catch (std::invalid_argument)
                    {
                        throw std::runtime_error(ConfigManager::getManagerMessagePrefix() + "Unable to parse configuration: " + THREAD_LIMIT_FIELD + " should be an integer!");
                    }
                    catch (std::out_of_range)
                    {
                        throw std::runtime_error(ConfigManager::getManagerMessagePrefix() + "Unable to parse configuration: " + THREAD_LIMIT_FIELD + " is out of range of integers!");
                    }
                }
                else
                {
                    thread_limit = THREAD_LIMIT_DEFAULT_VALUE;
                }
            }
        }
    }
}
