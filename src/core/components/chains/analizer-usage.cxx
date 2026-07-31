#include "analizer-usage.hh"
#include "core/components/chains/context/context.hh"
#include "core/components/chains/path-data.hh"
#include "utils/io_data/types/io_data_bool.hh"
#include "utils/io_data/types/io_data_int.hh"
#include "utils/io_data/types/io_data_float.hh"
#include "utils/io_data/types/io_data_string.hh"
#include "utils/logger/logger.hh"
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#define ANALIZER_USAGE_CATEGORY_NAME "AnalizerUsage"

#define PATH_DELIMITER '.'
#define REF_CHAR_START '$'

#define TRUE_VALUE_STR "true"
#define FALSE_VALUE_STR "false"

namespace core
{
    namespace components
    {
        namespace chains
        {
            AnalizerUsage::AnalizerUsage(analizer::Analizer& analizer, std::map<std::string, std::string> inputPath)
                : inputPath(inputPath)
            {
                this->analizer = analizer.clone();
            }

            AnalizerUsage::AnalizerUsage(const AnalizerUsage &old_usage)
                : inputPath(old_usage.inputPath)
            {
                this->analizer = old_usage.analizer->clone();

                std::cout << "Copyied" << std::endl;
            }

            std::map<std::string, std::unique_ptr<utils::io_data::IOData>> AnalizerUsage::process(Context& context, source::Source& source, bool& trigger)
            {
                std::map<std::string, utils::io_data::IOData *> inputs;

                for (auto input: inputPath)
                {
                    const std::string& input_name = input.first;
                    const std::string& input_path = input.second;

                    auto value = getValueFromPath(context, input_path);

                    if (value == nullptr)
                    {
                        continue;
                    }

                    inputs.insert({input_name, value});
                }

                return analizer->process(inputs, source, trigger);
            }

            utils::io_data::IOData* AnalizerUsage::getValueFromPath(Context& context, std::string path)
            {
                if (path.empty())
                {
                    utils::logger::Logger::instance().Log(ANALIZER_USAGE_CATEGORY_NAME, "empty value detected!", utils::logger::Logger::LogLevel::ERROR);

                    return nullptr;
                }

                if (path.at(0) == REF_CHAR_START)
                {
                    auto path_data = getPathData(path.substr(1));

                    if (!path_data.has_value())
                    {
                        std::ostringstream sb;
                        sb << "Invalid path detected: ";
                        sb << path;
                        sb << " !";

                        utils::logger::Logger::instance().Log(ANALIZER_USAGE_CATEGORY_NAME, sb.str(), utils::logger::Logger::LogLevel::ERROR);

                        return nullptr;
                    }

                    if (!context.hasVariable(path_data->getStage(), path_data->getAnalizerName(), path_data->getOutputName()))
                    {
                        std::ostringstream sb;
                        sb << "Invalid reference detected: ";
                        sb << path;
                        sb << " !";

                        utils::logger::Logger::instance().Log(ANALIZER_USAGE_CATEGORY_NAME, sb.str(), utils::logger::Logger::LogLevel::ERROR);

                        return nullptr;
                    }

                    return context.getVariable(path_data->getStage(), path_data->getAnalizerName(), path_data->getOutputName());
                }
                else
                {
                    auto bool_val = tryParseBool(path);

                    if (bool_val.has_value())
                    {
                        auto value = std::make_unique<utils::io_data::IODataBool>(bool_val.value());
                        auto value_ptr = value.get();

                        context.storeTemporaryValue(std::move(value));

                        return value_ptr;
                    }

                    auto int_val = tryParseInt(path);

                    if (int_val.has_value())
                    {
                        auto value = std::make_unique<utils::io_data::IODataInt>(int_val.value());
                        auto value_ptr = value.get();

                        context.storeTemporaryValue(std::move(value));

                        return value_ptr;
                    }

                    auto float_val = tryParseFloat(path);

                    if (float_val.has_value())
                    {
                        auto value = std::make_unique<utils::io_data::IODataFloat>(float_val.value());
                        auto value_ptr = value.get();

                        context.storeTemporaryValue(std::move(value));

                        return value_ptr;
                    }

                    auto value = std::make_unique<utils::io_data::IODataString>(path);
                    auto value_ptr = value.get();

                    context.storeTemporaryValue(std::move(value));

                    return value_ptr;
                }

                utils::logger::Logger::instance().Log(ANALIZER_USAGE_CATEGORY_NAME, "Unkown error detected in value parsing!", utils::logger::Logger::LogLevel::ERROR);

                return nullptr;
            }

            std::optional<bool> AnalizerUsage::tryParseBool(const std::string& value)
            {
                if (value == TRUE_VALUE_STR)
                {
                    return true;
                }

                if (value == FALSE_VALUE_STR)
                {
                    return false;
                }

                return std::nullopt;
            }

            std::optional<int> AnalizerUsage::tryParseInt(const std::string& value)
            {
                try
                {
                    return std::stoi(value);
                }
                catch (const std::invalid_argument&)
                {
                    return std::nullopt;
                }
                catch (const std::out_of_range&)
                {
                    return std::nullopt;
                }
            }

            std::optional<float> AnalizerUsage::tryParseFloat(const std::string& value)
            {
                try
                {
                    return std::stof(value);
                }
                catch (const std::invalid_argument&)
                {
                    return std::nullopt;
                }
                catch (const std::out_of_range&)
                {
                    return std::nullopt;
                }
            }

            bool AnalizerUsage::isValidCharForPath(char c)
            {
                if (c >= 'a' && c <= 'z')
                {
                    return true;
                }

                if (c >= 'A' && c <= 'Z')
                {
                    return true;
                }

                if (c >= '0' && c <= '9')
                {
                    return true;
                }

                if (c == '_' || c == '-' || c == '.')
                {
                    return true;
                }

                return false;
            }

            bool AnalizerUsage::isAcceptedPath(std::string& path)
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

            std::optional<PathData> AnalizerUsage::getPathData(std::string path)
            {
                if (!isAcceptedPath(path))
                {
                    return std::nullopt;
                }

                std::vector<std::string> parts = split(path, PATH_DELIMITER);

                if (parts.size() == 2)
                {
                    if (parts[0] != Context::INPUT_STAGE_NAME)
                    {
                        return std::nullopt;
                    }

                    return PathData(Context::INPUT_STAGE_INDEX, Context::INPUT_STAGE_NAME, parts[1]);
                }
                else if (parts.size() == 3)
                {
                    int stage_nb;

                    try
                    {
                        stage_nb = std::stoi(parts[0]);
                    }
                    catch (const std::invalid_argument&)
                    {
                        return std::nullopt;
                    }
                    catch (const std::out_of_range&)
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

            std::vector<std::string> AnalizerUsage::split(const std::string& input, char delimiter)
            {
                std::vector<std::string> res;
                size_t last_pos = 0;

                while (input.find(delimiter, last_pos) < input.length())
                {
                    size_t temp = input.find(delimiter, last_pos);
                    res.push_back(input.substr(last_pos, temp - last_pos));

                    last_pos = temp + 1;
                }

                if (last_pos < input.length())
                {
                    res.push_back(input.substr(last_pos));
                }

                return res;
            }
        }
    }
}
