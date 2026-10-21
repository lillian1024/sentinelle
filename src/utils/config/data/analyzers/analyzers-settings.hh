#pragma once

#include "core/components/analyzers/analyzer.hh"
#include "utils/config/data-module.hh"
#include <map>
#include <memory>
#include <string>

#define ANALIZERS_SETTINGS_FIELD_NAME "analyzers"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace analyzers
            {
                class AnalyzersSettings : public DataModule
                {
                    public:
                        AnalyzersSettings() = default;
                        ~AnalyzersSettings() = default;

                        void readModule(YAML::Node node);

                        bool hasAnalyzer(std::string name);

                        core::components::analyzer::Analyzer* getAnalyzer(std::string name);

                        std::map<std::string, std::reference_wrapper<core::components::analyzer::Analyzer>> getAnalyzersRefMap();

                        std::string dumpInfo();
                    protected:
                        std::map<std::string, std::unique_ptr<core::components::analyzer::Analyzer>> analyzers;
                };
            }
        }
    }
}
