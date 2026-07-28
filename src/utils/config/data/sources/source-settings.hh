#pragma once

#include <memory>
#include <opencv2/videoio.hpp>
#include <string>
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

                        virtual ~SourceSettings() = default;

                        static std::unique_ptr<SourceSettings> getSourceSettingsFromNode(YAML::Node node, std::string source_name);

                        virtual std::string dumpSettings() const = 0;
                        virtual SourceType getType() const = 0;

                        bool getIsLive() const
                        {
                            return is_live;
                        }

                        float getActiveFps() const
                        {
                            return active_fps;
                        }

                        float getPassiveFps() const
                        {
                            return passive_fps;
                        }

                        virtual cv::VideoCapture getVideoCapture() const = 0;

                        virtual std::unique_ptr<SourceSettings> clone() const = 0;

                        virtual bool getShowDebugView() const
                        {
                            return show_debug_view;
                        };

                        virtual std::string getName() const
                        {
                            return source_name;
                        }
                    protected:
                        SourceSettings(YAML::Node node, std::string source_name);

                        const std::string parent_field_name = "source settings";

                        std::string source_name;

                        bool is_live;

                        float active_fps;
                        float passive_fps;

                        bool show_debug_view;
                };
            }
        }
    }
}
