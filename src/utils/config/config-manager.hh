#pragma once

#include "utils/config/data/general-settings.hh"
#include <string>
#include <utils/singleton/singleton.hh>

namespace utils
{
    namespace config
    {
        /**
         * \brief This class is responsible for handling the loading of the configuration file.
         */
        class ConfigManager : public Singleton<ConfigManager>
        {
        public:
            /**
             * \brief Construct a new ConfigManager.
             */
            ConfigManager();

            /**
             * \brief force the reload of all the configuration from the configuration file.
             */
            void reloadSettings();

            /**
             * \brief Return the root configuration as a GeneralSettings.
             */
            data::GeneralSettings& getGeneralSettings()
            {
                return generalSettings;
            }

            /**
             * \brief The logging prefix of this function.
             */
            static const std::string managerMessagePrefix;
        protected:
            /**
             * \brief Name of the configuration file.
             */
            std::string configuration_file;
            /**
             * \brief The root configuration object.
             */
            data::GeneralSettings generalSettings;
        };
    }
}
