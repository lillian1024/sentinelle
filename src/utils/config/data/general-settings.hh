#pragma once

#include "utils/config/data-module.hh"
#include "utils/config/data/analizers/analizers-settings.hh"
#include "utils/config/data/chains/chains-settings.hh"
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
                inline analizers::AnalizersSettings& getAnalizersSettings() { return analizers_settings; }
                inline chains::ChainsSettings& getChainsSettings() { return chains_settings; }
            protected:
                std::string server_name;
                std::size_t thread_limit;
                logger::Logger::LogLevel logging_level;
                std::string cache_path;
                std::string log_file;
                sources::SourcesSettings sources_settings;
                analizers::AnalizersSettings analizers_settings;
                chains::ChainsSettings chains_settings;
            };
        }
    }
}
