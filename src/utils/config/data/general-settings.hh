#include "utils/config/data-module.hh"
#include "utils/config/data/sources/sources-settings.hh"
#include <cstddef>
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
                inline std::size_t getThreadLimit() { return thread_limit; }
                inline sources::SourcesSettings& getSourceSettings() { return sources_settings; }
            protected:
                std::string server_name;
                std::size_t thread_limit;
                sources::SourcesSettings sources_settings;
            };
        }
    }
}
