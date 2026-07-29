#pragma once

#include "core/components/chains/chain.hh"
#include "utils/config/data-module.hh"
#include <map>
#include <string>

#define CHAINS_SETTINGS_FIELD_NAME "chains"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace chains
            {
                class ChainsSettings : public DataModule
                {
                    public:
                        ChainsSettings() = default;
                        ~ChainsSettings() = default;

                        void readModule(YAML::Node node);

                        bool hasChain(std::string name);

                        core::components::chains::Chain& getChain(std::string name);

                        std::string dumpInfo();
                    protected:
                        std::map<std::string, core::components::chains::Chain> chains;
                };
            }
        }
    }
}
