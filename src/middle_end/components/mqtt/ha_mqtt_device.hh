#pragma once

#include "core/components/sources/source.hh"
#include <json/value.h>
#include <mosquitto/broker_plugin.h>
#include <string>

namespace middle_end
{
    namespace mqtt
    {
        #define TRIGGER_SENSOR_ID "trigger"
        #define ENABLED_SWITCH_ID "enabled"

        #define SWITCH_COMMAND_NAME "set"
        #define SWITCH_VALUE_NAME "value"

        #define SWITCH_ON_VALUE "ON"

        class MQTTSourceDevice
        {
            public:
                MQTTSourceDevice(core::components::source::Source& source);

                static const std::string topic_prefix;

                core::components::source::Source& getBaseSource() const;

                std::string getDiscoveryConfig() const;

                void subscribeToDeviceTopic(mosquitto *mosq) const;

                std::string getSerializedBoolState(std::string value_name, bool trigger) const;
            protected:
                Json::Value getDeviceTriggerComponent() const;
                Json::Value getDeviceEnabledComponent() const;

                std::string getStateTopic(std::string component_name) const;
                std::string getCommandTopic(std::string component_name, std::string command_name) const;

                core::components::source::Source& base_source;
        };
    }
}
