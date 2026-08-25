#pragma once

#include "middle_end/middle_end.hh"
#include "utils/config/data-module.hh"

#include <memory>
#include <string>
#include <vector>

#define MIDDLE_END_SETTINGS_FIELD_NAME "middle_end"

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace middle_end
            {
                class MiddleEndSettings : public DataModule
                {
                    public:
                        MiddleEndSettings() = default;
                        ~MiddleEndSettings() = default;

                        void readModule(YAML::Node node);

                        std::vector<std::reference_wrapper<::middle_end::MiddleEnd>> getMiddleEnds();

                        std::string dumpInfo();
                    protected:
                        std::vector<std::unique_ptr<::middle_end::MiddleEnd>> middle_ends;
                };
            }
        }
    }
}
