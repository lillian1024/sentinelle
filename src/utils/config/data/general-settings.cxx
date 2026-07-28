#include "general-settings.hh"

#include "sources/sources-settings.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data/analizers/analizers-settings.hh"
#include "utils/logger/logger.hh"
#include <optional>
#include <stdexcept>
#include <string>

#define PARENT_FIELD_NAME "general settings"
#define SERVER_NAME_FIELD "server_name"
#define THREAD_LIMIT_FIELD "thread_limit"
#define LOGGING_LEVEL_FIELD "log_level"
#define CACHE_PATH_FIELD "cache_path"
#define LOG_FILE_FIELD "log_file"

//Volontary underflow of size_t (unsigned) to get size_t's max value
#define DEFAULT_THREAD_LIMIT "-1"

#define DEFAULT_LOGGING_LEVEL "INFO"
#define DEFAULT_CACHE_PATH "./cache"
#define DEFAULT_LOG_FILE ""

namespace utils
{
    namespace config
    {
        namespace data
        {
            void GeneralSettings::readModule(YAML::Node node)
            {
                server_name = readScalarOrError(node, SERVER_NAME_FIELD, PARENT_FIELD_NAME);
                std::string thread_limit_str = readScalarOptional(node, THREAD_LIMIT_FIELD).value_or(DEFAULT_THREAD_LIMIT);

                std::string logging_level_str = readScalarOptional(node, LOGGING_LEVEL_FIELD).value_or(DEFAULT_LOGGING_LEVEL);
                log_file = readScalarOptional(node, LOG_FILE_FIELD).value_or(DEFAULT_LOG_FILE);
                cache_path = readScalarOptional(node, CACHE_PATH_FIELD).value_or(DEFAULT_CACHE_PATH);

                //Parse thread limit
                try
                {
                    //Volontary possible size_t (unsigned) underflow to get size_t's max value
                    thread_limit = std::stol(thread_limit_str);
                }
                catch (std::invalid_argument)
                {
                    throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse configuration: " + THREAD_LIMIT_FIELD + " should be an integer!");
                }
                catch (std::out_of_range)
                {
                    throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse configuration: " + THREAD_LIMIT_FIELD + " is out of range of integers!");
                }

                //Parse logging level
                std::optional<logger::Logger::LogLevel> log_level = logger::Logger::ParseLogLevel(logging_level_str);

                if (!log_level.has_value())
                {
                    throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse log level: " + logging_level_str + "!");
                }

                logging_level = log_level.value();

                sources_settings.readModule(readMapOrError(node, SOURCES_SETTINGS_FIELD_NAME, PARENT_FIELD_NAME));
                analizers_settings.readModule(readMapOrError(node, ANALIZERS_SETTINGS_FIELD_NAME, PARENT_FIELD_NAME));
            }
        }
    }
}
