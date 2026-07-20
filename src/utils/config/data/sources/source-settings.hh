#pragma once

#include <yaml-cpp/yaml.h>

#define SOURCE_TYPE_FIELD "type"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace sources
            {
                class SourceSettings
                {
                    public:
                        static SourceSettings getSourceSettingsFromNode(YAML::Node node);
                    protected:

                };
            }
        }
    }
}
