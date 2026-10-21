#include "chain.hh"

#include <map>
#include <opencv2/core/mat.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "core/components/analyzers/analyzer.hh"
#include "core/components/chains/analyzer-usage.hh"
#include "core/components/chains/context/context.hh"
#include "utils/logger/logger.hh"
#include "yaml-cpp/node/node.h"

#define CHAIN_CATEGORY_NAME "Chain"

namespace core
{
    namespace components
    {
        namespace chains
        {
            Chain::Chain(YAML::Node node,
                         std::map<std::string,
                                  std::reference_wrapper<analyzer::Analyzer>>
                             analyzers)
            {
                for (size_t i = 0; i < node.size(); i++)
                {
                    auto stage_node = node[i];

                    if (!stage_node.IsDefined())
                    {
                        std::string msg =
                            "Unable to parse chain: stages must be continous "
                            "number starting at 0 (e.g 0,1,2,3...)!";

                        utils::logger::Logger::instance().Log(
                            CHAIN_CATEGORY_NAME, msg,
                            utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    if (!stage_node.IsMap())
                    {
                        std::string msg =
                            "Unable to parse chain: stages must be maps "
                            "containing the analyzers to run!";

                        utils::logger::Logger::instance().Log(
                            CHAIN_CATEGORY_NAME, msg,
                            utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    stages.push_back(parseStage(stage_node, analyzers));
                }
            }

            std::map<std::string, AnalyzerUsage> Chain::parseStage(
                YAML::Node node,
                std::map<std::string,
                         std::reference_wrapper<analyzer::Analyzer>>
                    analyzers)
            {
                std::map<std::string, AnalyzerUsage> res;

                for (auto analyzer : node)
                {
                    if (!analyzer.second.IsMap())
                    {
                        std::string msg =
                            "Unable to parse chain: analyzers in stages must "
                            "be maps containing the input of the analyzer!";

                        utils::logger::Logger::instance().Log(
                            CHAIN_CATEGORY_NAME, msg,
                            utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    std::string analyzer_name = analyzer.first.Scalar();

                    auto ana = analyzers.find(analyzer_name);

                    if (ana == analyzers.end())
                    {
                        std::string msg =
                            "Unable to parse chain: Analyzer not found!";

                        utils::logger::Logger::instance().Log(
                            CHAIN_CATEGORY_NAME, msg,
                            utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    AnalyzerUsage usage(ana->second,
                                        parseAnalyzerinputs(analyzer.second));

                    if (res.contains(analyzer_name))
                    {
                        throw std::runtime_error(
                            "[Chain]: Duplicated analyzer name detected!");
                    }

                    res.insert({ analyzer_name, usage });
                }

                return res;
            }

            std::map<std::string, std::string>
            Chain::parseAnalyzerinputs(YAML::Node node)
            {
                std::map<std::string, std::string> res;

                for (auto input : node)
                {
                    if (!input.first.IsScalar() || !input.second.IsScalar())
                    {
                        std::string msg = "Unable to parse chain: input name "
                                          "and path should be a string!";

                        utils::logger::Logger::instance().Log(
                            CHAIN_CATEGORY_NAME, msg,
                            utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    std::string input_name = input.first.Scalar();
                    std::string input_path = input.second.Scalar();

                    res.insert({ input_name, input_path });
                }

                return res;
            }

            void Chain::process(cv::Mat& input_image, source::Source& source,
                                bool& trigger)
            {
                Context context(input_image, source.getName());

                for (size_t i = 0; i < stages.size(); i++)
                {
                    auto& stage = stages[i];

                    for (auto& analyzer : stage)
                    {
                        auto outputs =
                            analyzer.second.process(context, source, trigger);

                        for (auto& output : outputs)
                        {
                            context.setVariable(i, analyzer.first, output.first,
                                                std::move(output.second));
                        }
                    }
                }
            }

            std::string Chain::dumpConfig()
            {
                std::ostringstream sb;

                sb << "Number of stages: ";
                sb << stages.size();
                sb << '\n';

                for (size_t i = 0; i < stages.size(); i++)
                {
                    sb << "Stage #";
                    sb << i;
                    sb << '\n';
                    sb << "\tNumber of analyzers: ";
                    sb << stages[i].size();
                    sb << '\n';
                }

                return sb.str();
            }
        } // namespace chains
    } // namespace components
} // namespace core
