#include "chains-settings.hh"

#include <iostream>
#include <sstream>

#include "core/components/chains/chain.hh"
#include "utils/config/config-manager.hh"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace chains
            {
                void ChainsSettings::readModule(YAML::Node node)
                {
                    for (auto it = node.begin(); it != node.end(); it++)
                    {
                        if (!it->first.IsScalar())
                        {
                            throw std::runtime_error(
                                ConfigManager::managerMessagePrefix
                                + "Unable to parse chains: chain keys should "
                                  "be a string!");
                        }

                        std::string name = it->first.Scalar();

                        auto analyzers = ConfigManager::instance()
                                             .getGeneralSettings()
                                             .getAnalyzersSettings()
                                             .getAnalyzersRefMap();

                        chains.insert({ name,
                                        core::components::chains::Chain(
                                            it->second, analyzers) });
                    }
                }

                bool ChainsSettings::hasChain(std::string name)
                {
                    return chains.find(name) != chains.end();
                }

                core::components::chains::Chain&
                ChainsSettings::getChain(std::string name)
                {
                    return chains.find(name)->second;
                }

                std::string ChainsSettings::dumpInfo()
                {
                    std::ostringstream sb;

                    sb << "Chains number: ";
                    sb << chains.size();
                    sb << '\n';

                    for (auto it = chains.begin(); it != chains.end(); it++)
                    {
                        sb << it->second.dumpConfig();
                    }

                    return sb.str();
                }
            } // namespace chains
        } // namespace data
    } // namespace config
} // namespace utils
