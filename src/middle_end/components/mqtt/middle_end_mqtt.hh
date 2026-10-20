#pragma once

#include "core/event/event-handler.hh"
#include "core/event/events/event.hh"
#include "core/event/events/modules/trigger/triggered-event.hh"
#include "core/event/events/modules/trigger/untriggered-event.hh"
#include "middle_end/components/mqtt/ha_mqtt_device.hh"
#include "middle_end/middle_end.hh"
#include <cstddef>
#include <map>
#include <mosquitto.h>
#include <mosquitto.h>
#include <optional>
#include <string>
#include <vector>
#include <yaml-cpp/node/node.h>

namespace middle_end
{
    namespace mqtt
    {
        class MiddleEndMQTT : public MiddleEnd, core::event::EventHandler
        {
            public:
                MiddleEndMQTT(YAML::Node node);
                ~MiddleEndMQTT() = default;

                virtual void Init();
                virtual void Start();
                virtual void Stop();

                virtual void HandleDeviceCommand(std::string device_name, std::string component, std::string command, std::string payload);

                virtual void OnConnect(mosquitto *client, int response);
                virtual void OnDisconnect(mosquitto *client, int reason);

                static const std::string middle_end_mqtt_name;

                static std::vector<std::string> split(const std::string& input, char delimiter);
            protected:
                void handleEvent(core::event::Event& event);

                virtual void handleTriggerEvent(core::event::modules::TriggeredEvent& event);
                virtual void handleUnTriggerEvent(core::event::modules::UnTriggeredEvent& event);

                void sendDeviceState(MQTTSourceDevice device) const;
                void sendTriggerState(MQTTSourceDevice device, bool state) const;
                void sendEnabledState(MQTTSourceDevice device, bool state) const;

                std::string getResponseString(int response_code);
                std::string getReasonString(int response_code);

                bool enabled;

                mosquitto *client_instance;

                std::string broker_address;
                std::size_t broker_port;
                std::size_t broker_keepalive;

                std::optional<std::string> username;
                std::optional<std::string> password;

                std::map<std::string, MQTTSourceDevice> id_source_map;
                std::vector<std::string> sources_name;
        };

        void mqttMessageCallBack(struct mosquitto *mosq, void *obj, const mosquitto_message *message);
        void mqttConnectCallBack(struct mosquitto *mosq, void *obj, int response);
        void mqttDisconnectCallBack(struct mosquitto *mosq, void *obj, int reason);
    }
}
