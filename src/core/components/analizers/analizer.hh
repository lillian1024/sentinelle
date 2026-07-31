#pragma once

#include "core/components/sources/source.hh"
#include "utils/io_data/io_data.hh"
#include "yaml-cpp/node/node.h"
#include <map>
#include <memory>
#include <string>

namespace core
{
    namespace components
    {
        namespace analizer
        {
            enum class Type
            {
                DDN_GENERAL_IDENT,
                SHOW_IMAGE,
                SAVE_VIDEO,
            };

            class Analizer
            {
                public:
                    static const std::string ANALIZE_CATEGORY_NAME;

                    Analizer(std::string name);
                    virtual ~Analizer() = default;

                    static std::unique_ptr<Analizer> getAnalizerFromNode(YAML::Node, std::string name);

                    virtual std::map<std::string, utils::io_data::IODataType> getInputs() = 0;
                    virtual std::map<std::string, utils::io_data::IODataType> getOutputs() = 0;

                    virtual bool validateInputs(std::map<std::string, utils::io_data::IOData*> inputs);
                    virtual bool validateInputs(std::map<std::string, utils::io_data::IODataType> types);

                    template<class T>
                    T* getInputTypeFromInputs(std::map<std::string, utils::io_data::IOData*> inputs, std::string name);

                    virtual std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>, source::Source&, bool& trigger) = 0;

                    virtual inline std::string getName() { return name; }

                    virtual std::unique_ptr<Analizer> clone() = 0;
                protected:
                    std::string name;
            };
        }
    }
}

#include "analizer.hxx"
