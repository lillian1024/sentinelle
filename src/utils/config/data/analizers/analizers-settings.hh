#pragma once

#include "core/components/analizers/analizer.hh"
#include "utils/config/data-module.hh"
#include <map>
#include <memory>
#include <string>

#define ANALIZERS_SETTINGS_FIELD_NAME "analizers"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace analizers
            {
                class AnalizersSettings : public DataModule
                {
                    public:
                        AnalizersSettings() = default;
                        ~AnalizersSettings() = default;

                        void readModule(YAML::Node node);

                        bool hasAnalizer(std::string name);

                        core::components::analizer::Analizer* getAnalizer(std::string name);

                        std::string dumpInfo();
                    protected:
                        std::map<std::string, std::unique_ptr<core::components::analizer::Analizer>> analizers;
                };
            }
        }
    }
}
