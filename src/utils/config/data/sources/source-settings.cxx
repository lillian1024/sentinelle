#include "source-settings.hh"
#include "utils/config/data-module.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
#include <memory>
#include <stdexcept>
#include <string>

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace sources
            {
                std::unique_ptr<SourceSettings> SourceSettings::getSourceSettingsFromNode(YAML::Node node)
                {
                    std::string type_name = DataModule::readScalarOrError(node, SOURCE_TYPE_FIELD, "source settings");

                    if (type_name == UrlSourceSettings::URL_SOURE_TYPE_NAME)
                    {
                        return std::make_unique<UrlSourceSettings>(node);
                    }
                    else
                    {
                        throw std::runtime_error("[ConfigManager]: Unrecognized source type: " + type_name);
                    }
                }
            }
        }
    }
}
