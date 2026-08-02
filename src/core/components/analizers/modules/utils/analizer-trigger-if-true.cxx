#include "analizer-trigger-if-true.hh"
#include "core/components/analizers/analizer.hh"
#include "utils/io_data/io_data.hh"
#include "utils/io_data/types/io_data_bool.hh"
#include "utils/logger/logger.hh"

#include <memory>
#include <sstream>
#include <string>

#define INPUT_VALUE_NAME "input"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            AnalizerTriggerIfTrue::AnalizerTriggerIfTrue(std::string name)
                : Analizer(name)
            {

            }

            std::map<std::string, utils::io_data::IODataType> AnalizerTriggerIfTrue::getInputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                res.insert({INPUT_VALUE_NAME, utils::io_data::IODataType::BOOL});

                return res;
            }
            std::map<std::string, utils::io_data::IODataType> AnalizerTriggerIfTrue::getOutputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                return res;
            }

            std::map<std::string, std::unique_ptr<utils::io_data::IOData>> AnalizerTriggerIfTrue::process(std::map<std::string, utils::io_data::IOData*> inputs, source::Source& source, bool& trigger)
            {
                (void)source;

                auto input_data_ptr = inputs.find(INPUT_VALUE_NAME);

                if (input_data_ptr == inputs.end())
                {
                    std::ostringstream sb;

                    sb << "Missmatched arguments: missing '";
                    sb << INPUT_VALUE_NAME;
                    sb << "' argument!";

                    utils::logger::Logger::instance().Log("AnalizerTriggerIfTrue", sb.str(), utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                auto input_data = dynamic_cast<utils::io_data::IODataBool*>(input_data_ptr->second);

                if (input_data == nullptr)
                {
                    std::ostringstream sb;

                    sb << "Missmatched argument types: argument '";
                    sb << INPUT_VALUE_NAME;
                    sb << "' should be a boolean!";

                    utils::logger::Logger::instance().Log("AnalizerTriggerIfTrue", sb.str(), utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
                }

                bool value = input_data->getData();

                if (value)
                {
                    trigger = true;
                }

                return std::map<std::string, std::unique_ptr<utils::io_data::IOData>>();
            }

            std::unique_ptr<Analizer> AnalizerTriggerIfTrue::clone()
            {
                return std::make_unique<AnalizerTriggerIfTrue>(*this);
            }
        }
    }
}
