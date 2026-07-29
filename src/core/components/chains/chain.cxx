#include "chain.hh"
#include "core/components/analizers/analizer.hh"
#include "core/components/chains/analizer-usage.hh"
#include "core/components/chains/context/context.hh"
#include "utils/logger/logger.hh"
#include "yaml-cpp/node/node.h"
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#define PATH_DELIMITER '.'
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
                    if (!analizer.IsMap())
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

            std::map<std::string, PathData> Chain::parseAnalizerinputs(YAML::Node node)
            {
                std::map<std::string, PathData> res;

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

                    auto path_data = getPathData(input_path);

                    if (!path_data.has_value())
                    {
                        std::ostringstream msg_b;

                        msg_b << "Unable to parse chain: input path: \"";
                        msg_b << input_path;
                        msg_b << "\" is not valid!";

                        utils::logger::Logger::instance().Log(CHAIN_CATEGORY_NAME, msg_b.str(), utils::logger::Logger::LogLevel::CRITICAL);

                        std::ostringstream sb;

                        sb << '[';
                        sb << CHAIN_CATEGORY_NAME;
                        sb << "]: ";
                        sb << msg_b.str();

                        throw std::runtime_error(sb.str());
                    }

                    res.insert({input_name, path_data.value()});
                }

                return res;
            }

            bool Chain::isValidCharForPath(char c)
            {
                if (c >= 'a' && c <= 'z')
                {
                    return true;
                }

                if (c >= 'A' && c <= 'Z')
                {
                    return true;
                }

                if (c == '_' || c == '-' || c == '.')
                {
                    return true;
                }

                return false;
            }

            bool Chain::isAcceptedPath(std::string path)
            {
                if (path.size() < 3)
                {
                    return false;
                }

                for (auto c: path)
                {
                    if (!isValidCharForPath(c))
                    {
                        return false;
                    }
                }

                return true;
            }

            std::optional<PathData> Chain::getPathData(std::string path)
            {
                if (!isAcceptedPath(path))
                {
                    return std::nullopt;
                }

                std::vector<std::string> parts = split(path, PATH_DELIMITER);

                if (parts.size() != 2)
                {
                    if (parts[0] != Context::INPUT_STAGE_NAME)
                    {
                        return std::nullopt;
                    }

                    return PathData(Context::INPUT_STAGE_INDEX, Context::INPUT_STAGE_NAME, parts[1]);
                }
                else if (parts.size() != 3)
                {
                    int stage_nb;

                    try
                    {
                        stage_nb = std::stoi(parts[0]);
                    }
                    catch (std::invalid_argument)
                    {
                        return std::nullopt;
                    }
                    catch (std::out_of_range)
                    {
                        return std::nullopt;
                    }

                    return PathData(stage_nb, parts[1], parts[2]);
                }
                else
                {
                    return std::nullopt;
                }
            }

            std::vector<std::string> Chain::split(std::string input, char delimiter)
            {
                std::vector<std::string> res;
                size_t last_pos = 0;

                while (input.find(delimiter, last_pos) < input.length())
                {
                    size_t temp = input.find(delimiter, last_pos);
                    res.push_back(input.substr(last_pos, temp));

                    last_pos = temp;
                }

                return res;
            }
        }
    }
}
