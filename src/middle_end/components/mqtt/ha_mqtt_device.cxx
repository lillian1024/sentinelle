#include "ha_mqtt_device.hh"

#include <json/json.h>
#include <sstream>

#define TOPIC_PREFIX "homeassistant/sentinelle/";

#define TRIGGER_SENSOR_ID "trigger"

#define DEVICE_CATEGORY_NAME "dev"
#define ORIGIN_CATEGORY_NAME "o"
#define COMPONENTS_CATEGORY_NAME "cmps"
#define STATE_TOPIC_CATEGORY_NAME "state_topic"
#define QOS_CATEGORY_NAME "qos"

namespace middle_end
{
    namespace mqtt
    {
        MQTTSourceDevice::MQTTSourceDevice(core::components::source::Source& source)
            : base_source(source)
        {
        }

        core::components::source::Source& MQTTSourceDevice::getBaseSource() const
        {
            return base_source;
        }

        std::string MQTTSourceDevice::getDiscoveryConfig() const
        {
            Json::Value payload_root;
            Json::Value payload_device;
            Json::Value payload_origin;
            Json::Value payload_components;

            payload_device["ids"] = base_source.getName();
            payload_device["name"] = base_source.getName();
            payload_device["mf"] = "Sentinelle";
            payload_device["mdl"] = "source";
            payload_device["sw"] = "1.0";
            payload_device["sn"] = base_source.getName();
            payload_device["hw"] = "1.0rev1";

            payload_origin["name"] = "Sentinelle";
            payload_origin["sw"] = "1.0";
            payload_origin["url"] = "https://github.com/lillian1024/sentinelle";

            Json::Value trigger_state_comp = getDeviceComponents();

            payload_components[TRIGGER_SENSOR_ID] = trigger_state_comp;

            payload_root[DEVICE_CATEGORY_NAME] = payload_device;
            payload_root[ORIGIN_CATEGORY_NAME] = payload_origin;
            payload_root[COMPONENTS_CATEGORY_NAME] = payload_components;
            payload_root[QOS_CATEGORY_NAME] = 1;

            std::ostringstream sb;

            sb << payload_root;
            sb << std::endl;

            return sb.str();
        }

        Json::Value MQTTSourceDevice::getDeviceComponents() const
        {
            Json::Value trigger_state_comp;

            trigger_state_comp["p"] = "binary_sensor";
            //trigger_state_comp["device_class"] = "binary_sensor";
            trigger_state_comp["unique_id"] = base_source.getName() + "_" + TRIGGER_SENSOR_ID;

            std::ostringstream val_temp_sb;
            val_temp_sb << "{{ value_json.";
            val_temp_sb << TRIGGER_SENSOR_ID;
            val_temp_sb << " }}";
            trigger_state_comp["value_template"] = val_temp_sb.str();

            trigger_state_comp[STATE_TOPIC_CATEGORY_NAME] = getStateTopic(TRIGGER_SENSOR_ID);

            return trigger_state_comp;
        }

        std::string MQTTSourceDevice::getSerializedTriggerState(bool trigger) const
        {
            Json::Value payload_root;

            payload_root[TRIGGER_SENSOR_ID] = trigger ? "ON" : "OFF";

            std::ostringstream sb;
            sb << payload_root;

            return sb.str();
        }

        std::string MQTTSourceDevice::getStateTopic(std::string component_name) const
        {
            std::ostringstream state_topic_builder;

            state_topic_builder << TOPIC_PREFIX;
            state_topic_builder << base_source.getName();
            state_topic_builder << "/";
            state_topic_builder << component_name;
            state_topic_builder << "/";
            state_topic_builder << "state";

            return state_topic_builder.str();
        }
    }
}
