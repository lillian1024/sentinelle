#include "chain.hh"
#include "core/components/analizers/analizer.hh"
#include "core/components/chains/analizer-usage.hh"
#include "core/components/chains/context/context.hh"
#include "utils/logger/logger.hh"
#include "yaml-cpp/node/node.h"
#include <map>
#include <opencv2/core/mat.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#define CHAIN_CATEGORY_NAME "Chain"

namespace core
{
    namespace components
    {
        namespace chains
        {
            Chain::Chain(YAML::Node node, std::map<std::string, analizer::Analizer&> analizers)
            {
                for (size_t i = 0; i < node.size(); i++)
                {
                    auto stage_node = node[i];

                    if (!stage_node.IsDefined())
                    {
                        std::string msg = "Unable to parse chain: stages must be continous number starting at 0 (e.g 0,1,2,3...)!";

                        utils::logger::Logger::instance().Log(CHAIN_CATEGORY_NAME, msg, utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    if (!stage_node.IsMap())
                    {
                        std::string msg = "Unable to parse chain: stages must be maps containing the analizers to run!";

                        utils::logger::Logger::instance().Log(CHAIN_CATEGORY_NAME, msg, utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    stages.push_back(parseStage(stage_node, analizers));
                }
            }

            std::map<std::string, AnalizerUsage> Chain::parseStage(YAML::Node node, std::map<std::string, analizer::Analizer&> analizers)
            {
                std::map<std::string, AnalizerUsage> res;

                for (auto analizer: node)
                {
                    if (!analizer.second.IsMap())
                    {
                        std::string msg = "Unable to parse chain: analizers in stages must be maps containing the input of the analizer!";

                        utils::logger::Logger::instance().Log(CHAIN_CATEGORY_NAME, msg, utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    std::string analizer_name = analizer.first.Scalar();

                    auto ana = analizers.find(analizer_name);

                    if (ana == analizers.end())
                    {
                        std::string msg = "Unable to parse chain: Analizer not found!";

                        utils::logger::Logger::instance().Log(CHAIN_CATEGORY_NAME, msg, utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    AnalizerUsage usage(ana->second, parseAnalizerinputs(analizer.second));

                    res.insert({analizer_name, usage});
                }

                return res;
            }

            std::map<std::string, std::string> Chain::parseAnalizerinputs(YAML::Node node)
            {
                std::map<std::string, std::string> res;

                for (auto input: node)
                {
                    if (!input.first.IsScalar() || !input.second.IsScalar())
                    {
                        std::string msg = "Unable to parse chain: input name and path should be a string!";

                        utils::logger::Logger::instance().Log(CHAIN_CATEGORY_NAME, msg, utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg;

                        throw std::runtime_error(sb.str());
                    }

                    std::string input_name = input.first.Scalar();
                    std::string input_path = input.second.Scalar();

                    res.insert({input_name, input_path});
                }

                return res;
            }

            void Chain::process(cv::Mat& input_image, source::Source& source)
            {
                Context context(input_image, source.getName());

                for (size_t i = 0; i < stages.size(); i++)
                {
                    auto stage = stages[i];

                    for (auto& analizer : stage)
                    {
                        auto outputs = analizer.second.process(context, source);

                        for (auto& output: outputs)
                        {
                            context.setVariable(i, analizer.first, output.first, std::move(output.second));
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
                    sb << "\tNumber of analizers: ";
                    sb << stages[i].size();
                    sb << '\n';
                }

                return sb.str();
            }
        }
    }
}
