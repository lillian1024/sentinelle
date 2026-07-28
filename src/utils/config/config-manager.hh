#pragma once

#include "utils/config/data/general-settings.hh"
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

            data::GeneralSettings& getGeneralSettings()
            {
                return generalSettings;
            }

            static const std::string managerMessagePrefix;
        protected:
            std::string settings_file;
            data::GeneralSettings generalSettings;
        };
    }
}
