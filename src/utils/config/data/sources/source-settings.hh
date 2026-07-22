#pragma once

#include <memory>
#include <yaml-cpp/yaml.h>

#define SOURCE_TYPE_FIELD "type"
#define SHOW_DEBUG_VIEW_FIELD "show_debug_view"

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

                        virtual bool getShowDebugView()
                        {
                            return show_debug_view;
                        };
                    protected:
                        SourceSettings(YAML::Node node);

                        const std::string parent_field_name = "source settings";

                        bool show_debug_view;
                };
            }
        }
    }
}
