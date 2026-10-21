#include "analyzer-bool-delay.hh"

#include <memory>
#include <yaml-cpp/node/node.h>

#include "utils/config/data-module.hh"
#include "utils/io_data/types/io_data_bool.hh"
#include "utils/logger/logger.hh"

#define PARENT_NAME "AnalyzerBoolDelay"

#define START_VALUE_PROPERTY_NAME "start_value"
#define CHARGE_DELAY_PROPERTY_NAME "charge_delay"
#define DISCHARGE_DELAY_PROPERTY_NAME "discharge_delay"

#define START_VALUE_DEFAULT_VALUE "false"
#define CHARGE_DELAY_DEFAULT_VALUE "0"
#define DISCHARGE_DELAY_DEFAULT_VALUE "0"

#define INPUT_VALUE_NAME "input"
#define OUTPUT_VALUE_NAME "output"

namespace core
{
    namespace components
    {
        namespace analyzer
        {
            AnalyzerBoolDelay::AnalyzerBoolDelay(YAML::Node node,
                                                 std::string name)
                : Analyzer(name)
                , charge_status(0)
                , discharge_status(0)
            {
                start_value = utils::config::DataModule::readBool(
                    node, START_VALUE_PROPERTY_NAME, PARENT_NAME, true,
                    START_VALUE_DEFAULT_VALUE);

                charge_delay = utils::config::DataModule::readInt(
                    node, CHARGE_DELAY_PROPERTY_NAME, PARENT_NAME, true,
                    CHARGE_DELAY_DEFAULT_VALUE);
                discharge_delay = utils::config::DataModule::readInt(
                    node, DISCHARGE_DELAY_PROPERTY_NAME, PARENT_NAME, true,
                    DISCHARGE_DELAY_DEFAULT_VALUE);

                if (start_value)
                {
                    charge_status = charge_delay;
                }
                else
                {
                    discharge_status = discharge_delay;
                }
            }

            std::map<std::string, utils::io_data::IODataType>
            AnalyzerBoolDelay::getInputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                res.insert(
                    { INPUT_VALUE_NAME, utils::io_data::IODataType::BOOL });

                return res;
            }
            std::map<std::string, utils::io_data::IODataType>
            AnalyzerBoolDelay::getOutputs()
            {
                std::map<std::string, utils::io_data::IODataType> res;

                res.insert(
                    { OUTPUT_VALUE_NAME, utils::io_data::IODataType::BOOL });

                return res;
            }

            std::map<std::string, std::unique_ptr<utils::io_data::IOData>>
            AnalyzerBoolDelay::process(
                std::map<std::string, utils::io_data::IOData*> inputs,
                source::Source& source, bool&)
            {
                (void)source;

                auto input_data_ptr = inputs.find(INPUT_VALUE_NAME);

                if (input_data_ptr == inputs.end())
                {
                    std::ostringstream sb;

                    sb << "Missmatched arguments: missing '";
                    sb << INPUT_VALUE_NAME;
                    sb << "' argument!";

                    utils::logger::Logger::instance().Log(
                        "AnalyzerTriggerIfTrue", sb.str(),
                        utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string,
                                    std::unique_ptr<utils::io_data::IOData>>();
                }

                auto input_data = dynamic_cast<utils::io_data::IODataBool*>(
                    input_data_ptr->second);

                if (input_data == nullptr)
                {
                    std::ostringstream sb;

                    sb << "Missmatched argument types: argument '";
                    sb << INPUT_VALUE_NAME;
                    sb << "' should be a boolean!";

                    utils::logger::Logger::instance().Log(
                        "AnalyzerTriggerIfTrue", sb.str(),
                        utils::logger::Logger::LogLevel::ERROR);

                    return std::map<std::string,
                                    std::unique_ptr<utils::io_data::IOData>>();
                }

                bool value = input_data->getData();

                bool res_value;

                if (value)
                {
                    if (charge_status >= charge_delay)
                    {
                        // Charged
                        res_value = true;
                        discharge_status = 0;
                    }
                    else
                    {
                        // Charging
                        charge_status++;
                        res_value = false;
                    }
                }
                else
                {
                    if (discharge_status >= discharge_delay)
                    {
                        // Discharged
                        res_value = false;
                        charge_status = 0;
                    }
                    else
                    {
                        // Discharging
                        discharge_status++;
                        res_value = true;
                    }
                }

                std::map<std::string, std::unique_ptr<utils::io_data::IOData>>
                    res;

                res.insert({ OUTPUT_VALUE_NAME,
                             std::make_unique<utils::io_data::IODataBool>(
                                 res_value) });

                return res;
            }

            std::unique_ptr<Analyzer> AnalyzerBoolDelay::clone()
            {
                return std::make_unique<AnalyzerBoolDelay>(*this);
            }
        } // namespace analyzer
    } // namespace components
} // namespace core
