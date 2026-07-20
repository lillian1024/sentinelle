#pragma once

#include "data/include.hh"
#include <string>
#include <utils/singleton/singleton.hh>

namespace utils
{
    namespace config
    {
        class ConfigManager : public Singleton<ConfigManager>
        {
        public:
            ConfigManager();

            void reloadSettings();

            data::GeneralSettings getGeneralSettings()
            {
                return generalSettings;
            }
        protected:
            const std::string managerMessagePrefix = "[ConfigManager]: ";

            std::string settings_file;
            data::GeneralSettings generalSettings;
        };
    }
}
