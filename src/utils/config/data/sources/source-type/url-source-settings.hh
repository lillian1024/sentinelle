#pragma once

#include "utils/config/data/sources/source-settings.hh"
#include "yaml-cpp/node/node.h"
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
                    protected:
                        std::string url;
                        float active_fps;
                        float passive_fps;
                };
            }
        }
    }
}
