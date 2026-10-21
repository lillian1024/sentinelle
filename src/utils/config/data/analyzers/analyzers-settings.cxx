#include "analyzers-settings.hh"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "core/components/analyzers/analyzer.hh"
#include "utils/config/config-manager.hh"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace analyzers
            {
                void AnalyzersSettings::readModule(YAML::Node node)
                {
                    for (auto it = node.begin(); it != node.end(); it++)
                    {
                        if (!it->first.IsScalar())
                        {
                            throw std::runtime_error(
                                ConfigManager::managerMessagePrefix
                                + "Unable to parse analyzers: analyzer keys "
                                  "should be scalar!");
                        }

                        std::string name = it->first.Scalar();

                        analyzers.insert(
                            { name,
                              core::components::analyzer::Analyzer::
                                  getAnalyzerFromNode(it->second, name) });
                    }
                }

                bool AnalyzersSettings::hasAnalyzer(std::string name)
                {
                    return analyzers.find(name) != analyzers.end();
                }

                core::components::analyzer::Analyzer*
                AnalyzersSettings::getAnalyzer(std::string name)
                {
                    return analyzers.find(name)->second.get();
                }

                std::map<std::string,
                         std::reference_wrapper<
                             core::components::analyzer::Analyzer>>
                AnalyzersSettings::getAnalyzersRefMap()
                {
                    std::map<std::string,
                             std::reference_wrapper<
                                 core::components::analyzer::Analyzer>>
                        res;

                    for (auto& analyzer : analyzers)
                    {
                        res.insert({ analyzer.first, *analyzer.second.get() });
                    }

                    return res;
                }

                std::string AnalyzersSettings::dumpInfo()
                {
                    std::ostringstream sb;

                    sb << "Analyzers number: ";
                    sb << analyzers.size();
                    sb << '\n';

                    for (auto it = analyzers.begin(); it != analyzers.end();
                         it++)
                    {
                        sb << "\t- ";
                        sb << it->first;
                        sb << '\n';
                    }

                    return sb.str();
                }
            } // namespace analyzers
        } // namespace data
    } // namespace config
} // namespace utils
