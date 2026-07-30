#pragma once

#include <memory>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#define SOURCE_TYPE_FIELD "type"

namespace core
{
    namespace components
    {
        namespace chains
        {
            class Chain;
        }
    }
}


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

                        virtual ~SourceSettings();

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

                        virtual const std::vector<core::components::chains::Chain>& getProcess() const
                        {
                            return process;
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

                        std::vector<core::components::chains::Chain> process;
                };
            }
        }
    }
}
