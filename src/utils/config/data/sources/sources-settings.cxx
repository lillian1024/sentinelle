#include "sources-settings.hh"
#include "yaml-cpp/node/node.h"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#define SOURCES_LIST_INDEX "sources"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace sources
            {
                void SourcesSettings::readModule(YAML::Node node)
                {
                    const std::string msgPrefix = "[ConfigManager]: ";

                    if (!node.IsMap())
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: " + SOURCES_SETTINGS_FIELD_NAME + " field should be a map!");
                    }

                    YAML::Node source_list = node[SOURCES_LIST_INDEX];

                    if (!source_list.IsDefined())
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: " SOURCES_LIST_INDEX + " from " + SOURCES_SETTINGS_FIELD_NAME + " is not defined");
                    }

                    if (!source_list.IsMap())
                    {
                        throw std::runtime_error(msgPrefix + "Unable to parse config: " SOURCES_LIST_INDEX + " from " + SOURCES_SETTINGS_FIELD_NAME + " should be a sequence");
                    }

                    for (size_t i = 0; i < source_list.size(); i++)
                    {
                        YAML::Node c_source = source_list[i];

                        std::unique_ptr<SourceSettings> sourceData = SourceSettings::getSourceSettingsFromNode(c_source);

                        sources.push_back(std::move(sourceData));
                    }
                }
            }
        }
    }
}
