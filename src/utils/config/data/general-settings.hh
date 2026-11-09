#pragma once

#include "utils/config/data-module.hh"
#include "utils/config/data/analyzers/analyzers-settings.hh"
#include "utils/config/data/chains/chains-settings.hh"
#include "utils/config/data/middle-end/middle-end-settings.hh"
#include "utils/config/data/sources/sources-settings.hh"
#include "utils/logger/logger.hh"
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
                inline logger::Logger::LogLevel getLoggingLevel() { return logging_level; }
                inline std::string getCachePath() { return cache_path; }
                inline std::string getLogFile() { return log_file; }
                inline sources::SourcesSettings& getSourceSettings() { return sources_settings; }
                inline analyzers::AnalyzersSettings& getAnalyzersSettings() { return analyzers_settings; }
                inline chains::ChainsSettings& getChainsSettings() { return chains_settings; }
                inline middle_end::MiddleEndSettings& getMiddleEndSettings() { return middle_end_settings; }
            protected:
                /**
                 * \brief The name of the server.
                 */
                std::string server_name;
                /**
                 * \brief The limit of number of thread.
                 */
                std::size_t thread_limit;
                /**
                 * \brief The logging level to use in the logger.
                 */
                logger::Logger::LogLevel logging_level;
                /**
                 * \brief The path to the cache directory.
                 */
                std::string cache_path;
                /**
                 * \brief The file to insert the log into.
                 */
                std::string log_file;
                /**
                 * \brief The sources configuration.
                 */
                sources::SourcesSettings sources_settings;
                /**
                 * \brief The configuration of the analyzers.
                 */
                analyzers::AnalyzersSettings analyzers_settings;
                /**
                 * \brief The configuration of the chains.
                 */
                chains::ChainsSettings chains_settings;
                /**
                 * \brief The configuration of the middle ends.
                 */
                middle_end::MiddleEndSettings middle_end_settings;
            };
        }
    }
}
