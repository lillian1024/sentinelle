#include "source-settings.hh"
#include "utils/config/data-module.hh"
#include "utils/config/data/sources/source-type/url-source-settings.hh"
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
                SourceSettings SourceSettings::getSourceSettingsFromNode(YAML::Node node)
                {
                    std::string type_name = DataModule::readScalarOrError(node, SOURCE_TYPE_FIELD, "source settings");

                    if (type_name == UrlSourceSettings::URL_SOURE_TYPE_NAME)
                    {
                        return UrlSourceSettings(node);
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
