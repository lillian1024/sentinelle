#pragma once

#include "utils/config/data/sources/source-settings.hh"
#include "yaml-cpp/node/node.h"
#include <memory>
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
                        UrlSourceSettings(YAML::Node node, std::string source_name);
                        ~UrlSourceSettings() = default;

                        static const std::string URL_SOURE_TYPE_NAME;

                        std::string getUrl() const
                        {
                            return url;
                        }

                        virtual std::string dumpSettings() const
                        {
                            std::ostringstream string_builder;

                            string_builder << "\tType: " << URL_SOURE_TYPE_NAME << '\n';
                            string_builder << "\tShow Debug View: " << show_debug_view << '\n';
                            string_builder << "\tUrl: " << url << '\n';
                            string_builder << "\tActive fps: " << active_fps << '\n';
                            string_builder << "\tPassive fps: " << passive_fps << '\n';

                            return string_builder.str();
                        }

                        virtual std::unique_ptr<SourceSettings> clone() const;

                        virtual SourceSettings::SourceType getType() const;

                        cv::VideoCapture getVideoCapture() const;
                    protected:
                        std::string url;
                };
            }
        }
    }
}
