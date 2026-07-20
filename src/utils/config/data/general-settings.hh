#include "utils/config/data-module.hh"
#include "utils/config/data/sources/sources-settings.hh"
#include <string>

namespace utils
{
    namespace config
    {
        namespace data
        {
            class GeneralSettings : DataModule
            {
            public:
                GeneralSettings() = default;

                void readModule(YAML::Node node);

                inline std::string getServerName() { return server_name; }
                inline sources::SourcesSettings getSourceSettings() { return sources_settings; }
            protected:
                std::string server_name;
                sources::SourcesSettings sources_settings;
            };
        }
    }
}
