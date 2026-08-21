#include "analizers-settings.hh"
#include "core/components/analizers/analizer.hh"
#include "utils/config/config-manager.hh"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace analizers
            {
                void AnalizersSettings::readModule(YAML::Node node)
                {
                    for (auto it = node.begin(); it != node.end(); it++)
                    {
                        if (!it->first.IsScalar())
                        {
                            throw std::runtime_error(ConfigManager::managerMessagePrefix + "Unable to parse analizers: analizer keys should be scalar!");
                        }

                        std::string name = it->first.Scalar();

                        analizers.insert({name, core::components::analizer::Analizer::getAnalizerFromNode(it->second, name)});
                    }
                }

                bool AnalizersSettings::hasAnalizer(std::string name)
                {
                    return analizers.find(name) != analizers.end();
                }

                core::components::analizer::Analizer* AnalizersSettings::getAnalizer(std::string name)
                {
                    return analizers.find(name)->second.get();
                }

                std::map<std::string, std::reference_wrapper<core::components::analizer::Analizer>> AnalizersSettings::getAnalizersRefMap()
                {
                    std::map<std::string, std::reference_wrapper<core::components::analizer::Analizer>> res;

                    for (auto& analizer: analizers)
                    {
                        res.insert({analizer.first, *analizer.second.get()});
                    }

                    return res;
                }

                std::string AnalizersSettings::dumpInfo()
                {
                    std::ostringstream sb;

                    sb << "Analizers number: ";
                    sb << analizers.size();
                    sb << '\n';

                    for (auto it = analizers.begin(); it != analizers.end(); it++)
                    {
                        sb << "\t- ";
                        sb << it->first;
                        sb << '\n';
                    }

                    return sb.str();
                }
            }
        }
    }
}
