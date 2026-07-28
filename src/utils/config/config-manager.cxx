#include "config-manager.hh"
#include "yaml-cpp/exceptions.h"
#include "yaml-cpp/node/parse.h"
#include <cstdlib>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

#define DEFAULT_CONFIG_FILE "./config.yml"

namespace utils
{
    namespace config
    {
        const std::string ConfigManager::managerMessagePrefix = "[ConfigManager]: ";

        ConfigManager::ConfigManager()
            : generalSettings()
        {
            if (std::getenv("SENTINELLE_SETTINGS_FILE") == NULL)
            {
                settings_file = DEFAULT_CONFIG_FILE;
            }
            else
            {
                settings_file = std::getenv("SENTINELLE_SETTINGS_FILE");
            }
        }

        void ConfigManager::reloadSettings()
        {
            try
            {
                YAML::Node doc = YAML::LoadFile(settings_file);

                if (!doc.IsMap())
                {
                    throw std::runtime_error(managerMessagePrefix + "Unable to read config: root yaml should be a map!");
                }

                generalSettings.readModule(doc);
            }
            catch (const YAML::BadFile& e)
            {
                throw std::runtime_error(managerMessagePrefix + "Unable to read configuration file: " + settings_file + ", check that the file exist and has the right permissions!");
            }
            catch (const YAML::ParserException& e)
            {
                throw std::runtime_error(managerMessagePrefix + "Unable to read configuration file: file is malformated!");
            }
        }
    }
}
