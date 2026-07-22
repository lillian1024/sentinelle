#pragma once

#include "utils/config/data/sources/source-settings.hh"
#include "yaml-cpp/node/node.h"
#include <fstream>
#include <sstream>
#include <string>
#include <yaml-cpp/yaml.h>

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace sources
            {
                class UrlSourceSettings : public SourceSettings
                {
                    public:
                        UrlSourceSettings(YAML::Node node);

                        static const std::string URL_SOURE_TYPE_NAME;

                        std::string getUrl()
                        {
                            return url;
                        }

                        float getActiveFps()
                        {
                            return active_fps;
                        }

                        float getPassiveFps()
                        {
                            return passive_fps;
                        }

                        virtual std::string dumpSettings()
                        {
                            std::ostringstream string_builder;

                            string_builder << "\tType: " << URL_SOURE_TYPE_NAME << '\n';
                            string_builder << "\tShow Debug View: " << show_debug_view << '\n';
                            string_builder << "\tUrl: " << url << '\n';
                            string_builder << "\tActive fps: " << active_fps << '\n';
                            string_builder << "\tPassive fps: " << passive_fps << '\n';

                            return string_builder.str();
                        }

                        virtual SourceSettings::SourceType getType();
                    protected:
                        std::string url;
                        float active_fps;
                        float passive_fps;
                };
            }
        }
    }
}
