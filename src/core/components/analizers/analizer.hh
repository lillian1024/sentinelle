#include "utils/io_data/io_data.hh"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace core
{
    namespace components
    {
        namespace analizer
        {
            class Analizer
            {
                public:
                    static const std::string ANALIZE_CATEGORY_NAME;

                    virtual std::map<std::string, utils::io_data::IODataType> getInputs() = 0;
                    virtual std::map<std::string, utils::io_data::IODataType> getOutputs() = 0;

                    virtual bool validateInputs(std::map<std::string, utils::io_data::IOData*> inputs);
                    virtual bool validateInputs(std::map<std::string, utils::io_data::IODataType> types);

                    virtual std::map<std::string, std::unique_ptr<utils::io_data::IOData>> process(std::map<std::string, utils::io_data::IOData*>) = 0;
            };
        }
    }
}
