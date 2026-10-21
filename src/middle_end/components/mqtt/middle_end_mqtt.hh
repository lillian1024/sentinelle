#pragma once

#include "core/event/event-handler.hh"
#include "core/event/events/event.hh"
#include "core/event/events/modules/source-start-event.hh"
#include "core/event/events/modules/trigger/triggered-event.hh"
#include "core/event/events/modules/trigger/untriggered-event.hh"
#include "core/event/events/modules/source-stop-event.hh"
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
        /**
         * \brief This middle-end allows communication through the MQTT protocol using the Home Assistant convention.
         * \details This middle-end can be used to control or get status update for the sources throught MQTT with any other devices but it follows the Home Assistant MQTT convention (https://www.home-assistant.io/integrations/mqtt).
         */
        class MiddleEndMQTT : public MiddleEnd, core::event::EventHandler
        {
            public:
                /**
                 * \brief Create new MQTT middle-end from configuration.
                 */
                MiddleEndMQTT(YAML::Node node);
                ~MiddleEndMQTT() = default;

                virtual void Init();
                virtual void Start();
                virtual void Stop();

                virtual void AttachToRunning();

                /**
                 * \brief This method is called by the mqtt message receive callback to handle a device command.
                 */
                virtual void HandleDeviceCommand(std::string device_name, std::string component, std::string command, std::string payload);

                /**
                 * \brief This method is called by the mqtt on connect callback.
                 */
                virtual void OnConnect(mosquitto *client, int response);
                /**
                 * \brief This method is called by the mqtt on disconnect callback.
                 */
                virtual void OnDisconnect(mosquitto *client, int reason);

                /**
                 * \brief The MQTT middle-end category name for logging purposes.
                 */
                static const std::string middle_end_mqtt_name;

                /**
                 * \brief Split a string input a list of strings delimited by the delimiter.
                 * \details The delimiter is removed from the strings.
                 */
                static std::vector<std::string> split(const std::string& input, char delimiter);
            protected:
                void handleEvent(core::event::Event& event);

                virtual void handleTriggerEvent(core::event::modules::TriggeredEvent& event);
                virtual void handleUnTriggerEvent(core::event::modules::UnTriggeredEvent& event);
                virtual void handleSourceStopEvent(core::event::modules::SourceStopEvent& event);
                virtual void handleSourceStartEvent(core::event::modules::SourceStartEvent& event);

                void sendDeviceState(MQTTSourceDevice device) const;
                void sendDeviceStatus(MQTTSourceDevice device, bool status) const;

                void sendTriggerState(MQTTSourceDevice device, bool state) const;
                void sendTriggerState(MQTTSourceDevice device, std::string state_payload) const;

                void sendTriggerStatus(MQTTSourceDevice device, std::string state_payload) const;

                void sendEnabledState(MQTTSourceDevice device, bool state) const;
                void sendEnabledState(MQTTSourceDevice device, std::string state_payload) const;

                void sendEnabledStatus(MQTTSourceDevice device, std::string state_payload) const;

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
