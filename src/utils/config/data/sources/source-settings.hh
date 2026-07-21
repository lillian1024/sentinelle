#pragma once

#include <memory>
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
                        static std::unique_ptr<SourceSettings> getSourceSettingsFromNode(YAML::Node node);

                        virtual std::string dumpSettings() = 0;
                    protected:

                };
            }
        }
    }
}
