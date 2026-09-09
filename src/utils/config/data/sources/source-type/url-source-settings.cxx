#include "url-source-settings.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data-module.hh"
#include "utils/config/data/sources/source-settings.hh"
#include <memory>
#include <opencv2/videoio.hpp>
#include <stdexcept>
#include <string>

#define URL_FIELD "url"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace sources
            {
                const std::string UrlSourceSettings::URL_SOURE_TYPE_NAME = "url";

                UrlSourceSettings::UrlSourceSettings(YAML::Node node, std::string source_name)
                    : SourceSettings(node, source_name)
                {
                    if (!node.IsDefined())
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: error while parsing source!");
                    }

                    if (DataModule::readScalarOrError(node, SOURCE_TYPE_FIELD, parent_field_name) != URL_SOURE_TYPE_NAME)
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: mismatched source type! please report this error to the maintainers.");
                    }

                    url = DataModule::readScalarOrError(node, URL_FIELD, parent_field_name);
                }

                 SourceSettings::SourceType UrlSourceSettings::getType() const
                 {
                     return SourceSettings::SourceType::URL;
                 }

                 cv::VideoCapture UrlSourceSettings::getVideoCapture() const
                 {
                     return cv::VideoCapture(url, cv::CAP_FFMPEG);
                 }

                 std::unique_ptr<SourceSettings> UrlSourceSettings::clone() const
                 {
                     return std::make_unique<UrlSourceSettings>(*this);
                 }

                 std::string UrlSourceSettings::dumpSettings() const
                 {
                     std::ostringstream string_builder;

                     string_builder << "\tType: " << URL_SOURE_TYPE_NAME << '\n';
                     string_builder << "\tUrl: " << url << '\n';
                     string_builder << "\tActive fps: " << active_fps << '\n';
                     string_builder << "\tPassive fps: " << passive_fps << '\n';
                     string_builder << "\tProcess #: " << process.size() << '\n';

                     return string_builder.str();
                 }
            }
        }
    }
}
