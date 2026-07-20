#include "general-settings.hh"

#include "sources/sources-settings.hh"

#define SERVER_NAME_FIELD "server_name"

namespace utils
{
    namespace config
    {
        namespace data
        {
            void GeneralSettings::readModule(YAML::Node node)
            {
                server_name = readScalarOrError(node, SERVER_NAME_FIELD, "general settings");
                sources_settings.readModule(readMapOrError(node, SOURCES_SETTINGS_FIELD_NAME, "general settings"));
            }
        }
    }
}
