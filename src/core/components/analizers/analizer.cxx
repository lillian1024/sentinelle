#include "analizer.hh"

#include "utils/io_data/io_data.hh"
#include "utils/logger/logger.hh"
#include <map>
#include <string>

#define GENERIC_ANALIZER_CATEGORY_NAME "AnalizerDNNGenericIdent"

namespace core
{
    namespace components
    {
        namespace analizer
        {
            const std::string Analizer::ANALIZE_CATEGORY_NAME = "Analizer";

            bool Analizer::validateInputs(std::map<std::string, utils::io_data::IOData*> inputs)
            {
                auto inputTypes = getInputs();

                if (inputTypes.size() != inputs.size())
                {
                    utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to process analizer: missmatched number of arguments!", utils::logger::Logger::LogLevel::ERROR);

                    return false;
                }

                for (auto pair: inputTypes)
                {
                    if (pair.second != inputs[pair.first]->getType())
                    {
                        utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to process analizer: missmatched argument types!", utils::logger::Logger::LogLevel::ERROR);

                        return false;
                    }
                }

                return true;
            }

            bool Analizer::validateInputs(std::map<std::string, utils::io_data::IODataType> types)
            {
                auto inputTypes = getInputs();

                if (inputTypes.size() != types.size())
                {
                    utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to process analizer: missmatched number of arguments!", utils::logger::Logger::LogLevel::ERROR);

                    return false;
                }

                for (auto pair: inputTypes)
                {
                    if (pair.second != types[pair.first])
                    {
                        utils::logger::Logger::instance().Log(ANALIZE_CATEGORY_NAME, "Unable to process analizer: missmatched argument types!", utils::logger::Logger::LogLevel::ERROR);

                        return false;
                    }
                }

                return true;
            }
        }
    }
}
