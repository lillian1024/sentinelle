#include "url-source-settings.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data-module.hh"
#include "utils/config/data/sources/source-settings.hh"
#include <memory>
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

                UrlSourceSettings::UrlSourceSettings(YAML::Node node)
                    : SourceSettings(node)
                {
                    if (!node.IsDefined())
                    {
                        throw std::runtime_error(ConfigManager::getManagerMessagePrefix() + "Unable to parse config: error while parsing source!");
                    }

                    if (DataModule::readScalarOrError(node, SOURCE_TYPE_FIELD, parent_field_name) != URL_SOURE_TYPE_NAME)
                    {
                        throw std::runtime_error(ConfigManager::getManagerMessagePrefix() + "Unable to parse config: mismatched source type! please report this error to the maintainers.");
                    }

                    url = DataModule::readScalarOrError(node, URL_FIELD, parent_field_name);
                }

                 SourceSettings::SourceType UrlSourceSettings::getType() const
                 {
                     return SourceSettings::SourceType::URL;
                 }

                 cv::VideoCapture UrlSourceSettings::getVideoCapture() const
                 {
                     return cv::VideoCapture(url);
                 }

                 std::unique_ptr<SourceSettings> UrlSourceSettings::clone() const
                 {
                     return std::make_unique<UrlSourceSettings>(*this);
                 }
            }
        }
    }
}
