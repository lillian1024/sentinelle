#pragma once

#include "utils/config/data-module.hh"
#include "utils/config/data/sources/source-settings.hh"
#include <yaml-cpp/yaml.h>
#include <vector>

#define SOURCES_SETTINGS_FIELD_NAME "sources_settings"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace sources
            {
                class SourcesSettings : DataModule
                {
                    public:
                        SourcesSettings() = default;

                        void readModule(YAML::Node node);

                        std::vector<SourceSettings> sources;
                    protected:
                };
            }
        }
    }
}
