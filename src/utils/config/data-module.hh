#pragma once

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
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field " + property_name + " of " + parent_name + " is not defined!");
                }

                if (!node[property_name].IsScalar())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field " + property_name + " of " + parent_name + " is not a scalar!");
                }

                return node[property_name].Scalar();
            };

            inline static YAML::Node readMapOrError(YAML::Node node, std::string property_name, std::string parent_name)
            {
                if (!node[property_name].IsDefined())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field " + property_name + " of " + parent_name + " is not defined!");
                }

                if (!node[property_name].IsMap())
                {
                    throw std::runtime_error("[ConfigManager]: Unable to parse config: the field " + property_name + " of " + parent_name + " is not a map!");
                }

                return node[property_name];
            };
        protected:

        };
    }
}
