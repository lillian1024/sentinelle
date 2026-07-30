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

                        virtual std::string dumpSettings() const;

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
