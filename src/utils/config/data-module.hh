#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <yaml-cpp/yaml.h>

namespace utils
{
    namespace config
    {
        class DataModule
        {
        public:
            virtual void readModule(YAML::Node node) = 0;

            inline static std::string readScalarOrError(YAML::Node node, std::string property_name, std::string parent_name)
            {
                if (!node[property_name].IsDefined())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field '" + property_name + "' of " + parent_name + " is not defined!");
                }

                if (!node[property_name].IsScalar())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field '" + property_name + "' of " + parent_name + " is not a string!");
                }

                return node[property_name].Scalar();
            };

            inline static std::optional<std::string> readScalarOptional(YAML::Node node, std::string property_name)
            {
                if (!node[property_name].IsDefined())
                {
                    return std::nullopt;
                }

                if (!node[property_name].IsScalar())
                {
                    return std::nullopt;
                }

                return node[property_name].Scalar();
            };

            inline static YAML::Node readMapOrError(YAML::Node node, std::string property_name, std::string parent_name)
            {
                if (!node[property_name].IsDefined())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field '" + property_name + "' of " + parent_name + " is not defined!");
                }

                if (!node[property_name].IsMap())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field '" + property_name + "' of " + parent_name + " is not a map!");
                }

                return node[property_name];
            };

            inline static YAML::Node readSequenceOrError(YAML::Node node, std::string property_name, std::string parent_name)
            {
                if (!node[property_name].IsDefined())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field '" + property_name + "' of " + parent_name + " is not defined!");
                }

                if (!node[property_name].IsSequence())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field '" + property_name + "' of " + parent_name + " is not a sequence!");
                }

                return node[property_name];
            };

            inline static long int readInt(YAML::Node node, std::string property_name, std::string parent_name, bool is_optional, std::string default_value)
            {
                std::string value_str;

                if (is_optional)
                {
                    value_str = readScalarOptional(node, property_name).value_or(default_value);
                }
                else
                {
                    value_str = readScalarOrError(node, property_name, parent_name);
                }

                try
                {
                    return std::stol(value_str);
                }
                catch (const std::invalid_argument&)
                {
                    std::string prefix = "[";

                    throw std::runtime_error(prefix + parent_name + "]: " + property_name + " must be an integer!");
                }
                catch (const std::out_of_range&)
                {
                    std::string prefix = "[";

                    throw std::runtime_error(prefix + parent_name + "]: " + property_name + " value is too big for type long integer!");
                }
            }
            inline static double readDouble(YAML::Node node, std::string property_name, std::string parent_name, bool is_optional, std::string default_value)
            {
                std::string value_str;

                if (is_optional)
                {
                    value_str = readScalarOptional(node, property_name).value_or(default_value);
                }
                else
                {
                    value_str = readScalarOrError(node, property_name, parent_name);
                }

                try
                {
                    return std::stod(value_str);
                }
                catch (const std::invalid_argument&)
                {
                    std::string prefix = "[";

                    throw std::runtime_error(prefix + parent_name + "]: " + property_name + " must be an integer!");
                }
                catch (const std::out_of_range&)
                {
                    std::string prefix = "[";

                    throw std::runtime_error(prefix + parent_name + "]: " + property_name + " value is too big for type long integer!");
                }
            }
            inline static bool readBool(YAML::Node node, std::string property_name, std::string parent_name, bool is_optional, std::string default_value)
            {
                std::string value_str;

                if (is_optional)
                {
                    value_str = readScalarOptional(node, property_name).value_or(default_value);
                }
                else
                {
                    value_str = readScalarOrError(node, property_name, parent_name);
                }

                transform(value_str.begin(), value_str.end(), value_str.begin(), ::tolower);

                if (value_str != "true" && value_str != "false")
                {
                    std::string prefix = "[";

                    throw std::runtime_error(prefix + parent_name + "]: " + property_name + " must be eight 'true' or 'false'!");
                }

                return value_str == "true";
            }
        protected:

        };
    }
}
