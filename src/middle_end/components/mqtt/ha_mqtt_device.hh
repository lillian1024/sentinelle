#pragma once

#include "core/components/sources/source.hh"
#include <json/value.h>
#include <string>
namespace middle_end
{
    namespace mqtt
    {
        class MQTTSourceDevice
        {
            public:
                MQTTSourceDevice(core::components::source::Source& source);

                core::components::source::Source& getBaseSource() const;

                std::string getDiscoveryConfig() const;
            protected:
                Json::Value getDeviceComponents() const;

                std::string getSerializedTriggerState(bool trigger) const;

                std::string getStateTopic(std::string component_name) const;

                core::components::source::Source& base_source;
        };
    }
}
