#include "sources-settings.hh"
#include "utils/config/config-manager.hh"
#include "yaml-cpp/node/node.h"
#include <memory>
#include <stdexcept>
#include <string>
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
                    if (!node.IsMap())
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: " + SOURCES_SETTINGS_FIELD_NAME + " field should be a map!");
                    }

                    YAML::Node source_list = node[SOURCES_LIST_INDEX];

                    if (!source_list.IsDefined())
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: " SOURCES_LIST_INDEX + " from " + SOURCES_SETTINGS_FIELD_NAME + " is not defined");
                    }

                    if (!source_list.IsMap())
                    {
                        throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: " SOURCES_LIST_INDEX + " from " + SOURCES_SETTINGS_FIELD_NAME + " should be a sequence");
                    }

                    for (auto it = source_list.begin(); it != source_list.end(); it++)
                    {
                        YAML::Node key = it->first;
                        YAML::Node c_source = it->second;

                        if (!key.IsScalar())
                        {
                            throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse config: " SOURCES_LIST_INDEX + " from " + SOURCES_SETTINGS_FIELD_NAME + " contained elements with scalar keys!");
                        }

                        std::unique_ptr<SourceSettings> sourceData = SourceSettings::getSourceSettingsFromNode(c_source, key.Scalar());

                        sources.push_back(std::move(sourceData));
                    }
                }
            }
        }
    }
}
