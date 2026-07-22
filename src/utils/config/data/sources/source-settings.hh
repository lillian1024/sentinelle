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
                        enum class SourceType
                        {
                            URL,
                        };

                        static std::unique_ptr<SourceSettings> getSourceSettingsFromNode(YAML::Node node);

                        virtual std::string dumpSettings() = 0;
                        virtual SourceType getType() = 0;
                    protected:

                };
            }
        }
    }
}
