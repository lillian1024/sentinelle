#include "middle_end_mqtt.hh"

#include <cstddef>
#include <json/json.h>
#include <mosquitto.h>
#include <sstream>
#include <stdexcept>
#include <string>

#include "core/event/event-manager.hh"
#include "core/event/events/event.hh"
#include "core/event/events/modules/source-start-event.hh"
#include "core/event/events/modules/source-stop-event.hh"
#include "core/event/events/modules/trigger/triggered-event.hh"
#include "core/event/events/modules/trigger/untriggered-event.hh"
#include "core/orchestrator/orchestrator.hh"
#include "middle_end/components/mqtt/ha_mqtt_device.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data-module.hh"
#include "utils/logger/logger.hh"

#define TOPIC_PATH_DELIMITER '/'

#define DEVICE_NAME_PATH_POSITION 2
#define COMPONENT_PATH_POSITION 3
#define COMMAND_PATH_POSITION 4

#define ENABLED_NAME "enable"
#define BROKER_ADDRESS_NAME "broker_address"
#define BROKER_PORT_NAME "broker_port"
#define BROKER_KEEPALIVE_NAME "broker_keepalive"

#define USERNAME_NAME "username"
#define PASSWORD_NAME "password"

#define BROKER_KEEPALIVE_DEFAULT "60"

#define MANAGED_SOURCE_LIST_NAME "sources"

#define CATEGORY_NAME "MiddleEndMQTT"

namespace middle_end
{
    namespace mqtt
    {
        const std::string MiddleEndMQTT::middle_end_mqtt_name = "ha_mqtt";

        MiddleEndMQTT::MiddleEndMQTT(YAML::Node node)
        {
            client_instance = nullptr;
            enabled = utils::config::DataModule::readScalarOrError(
                          node, ENABLED_NAME, middle_end_mqtt_name)
                == "true";
            broker_address = utils::config::DataModule::readScalarOrError(
                node, BROKER_ADDRESS_NAME, middle_end_mqtt_name);

            std::string broker_port_str =
                utils::config::DataModule::readScalarOrError(
                    node, BROKER_PORT_NAME, middle_end_mqtt_name);
            std::string broker_keepalive_str =
                utils::config::DataModule::readScalarOptional(
                    node, BROKER_KEEPALIVE_NAME)
                    .value_or(BROKER_KEEPALIVE_DEFAULT);

            username = utils::config::DataModule::readScalarOptional(
                node, USERNAME_NAME);
            password = utils::config::DataModule::readScalarOptional(
                node, PASSWORD_NAME);

            if (username.has_value() != password.has_value())
            {
                throw std::runtime_error(
                    "[MiddleEndMQTT]: If username or password is specified the "
                    "other one must also be specified!");
            }

            auto source_list = utils::config::DataModule::readSequenceOrError(
                node, MANAGED_SOURCE_LIST_NAME, middle_end_mqtt_name);

            try
            {
                broker_port = std::stoi(broker_port_str);
                broker_keepalive = std::stoi(broker_keepalive_str);
            }
            catch (const std::invalid_argument&)
            {
                throw std::runtime_error(
                    "[MiddleEndMQTT]: broker_port and broker_keepalive should "
                    "be integers!");
            }
            catch (const std::out_of_range&)
            {
                throw std::runtime_error(
                    "[MiddleEndMQTT]: Integer for broker_port and "
                    "broker_keepalive should not be too big!");
            }

            for (size_t i = 0; i < source_list.size(); i++)
            {
                if (!source_list[i].IsScalar())
                {
                    throw std::runtime_error(
                        "[MiddleEndMQTT]: elements in sources of mqtt should "
                        "be strings!");
                }

                auto source_name = source_list[i].Scalar();

                sources_name.push_back(source_name);
            }
        }

        void MiddleEndMQTT::Init()
        {
            // Init mosquitto lib before any other function
            mosquitto_lib_init();

            for (auto source_name : sources_name)
            {
                auto* source = core::orchestrator::Orchestrator::instance()
                                   .GetSourceByName(source_name);

                if (source == nullptr)
                {
                    std::ostringstream sb;
                    sb << "[MiddleEndMQTT]: source from mqtt config not "
                          "found: ";
                    sb << source_name;
                    sb << "!";

                    throw std::runtime_error(sb.str());
                }

                MQTTSourceDevice dev(*source);

                id_source_map.insert({ dev.getBaseSource().getName(), dev });
            }
        }

        void MiddleEndMQTT::Start()
        {
            client_instance =
                mosquitto_new(utils::config::ConfigManager::instance()
                                  .getGeneralSettings()
                                  .getServerName()
                                  .c_str(),
                              true, this);

            mosquitto_connect_callback_set(client_instance,
                                           &mqttConnectCallBack);
            mosquitto_disconnect_callback_set(client_instance,
                                              &mqttDisconnectCallBack);

            if (client_instance == nullptr)
            {
                utils::logger::Logger::instance().Log(
                    CATEGORY_NAME, "Unable to start the mqtt client instance!",
                    utils::logger::Logger::LogLevel::ERROR);

                return;
            }

            if (username.has_value())
            {
                int pass_status = mosquitto_username_pw_set(
                    client_instance, username->c_str(), password->c_str());

                if (pass_status != MOSQ_ERR_SUCCESS)
                {
                    utils::logger::Logger::instance().Log(
                        CATEGORY_NAME,
                        "Error while specifying the username and password!",
                        utils::logger::Logger::LogLevel::ERROR);

                    return;
                }
            }

            int connect_status =
                mosquitto_connect(client_instance, broker_address.c_str(),
                                  broker_port, broker_keepalive);

            if (connect_status != MOSQ_ERR_SUCCESS)
            {
                utils::logger::Logger::instance().Log(
                    CATEGORY_NAME, "Unable to connect to the MQTT broker!",
                    utils::logger::Logger::LogLevel::ERROR);

                enabled = false;

                return;
            }

            std::string topic_prefix = "homeassistant/device/";

            for (auto id_source : id_source_map)
            {
                auto device_id = id_source.second.getBaseSource().getName();

                std::ostringstream sb;

                sb << topic_prefix;
                sb << device_id;
                sb << "/config";

                std::string payload = id_source.second.getDiscoveryConfig();

                utils::logger::Logger::instance().Log(
                    "MiddleEndMQTT", "Advertising new MQTT device: ",
                    utils::logger::Logger::LogLevel::DEBUG);
                utils::logger::Logger::instance().LogPlain(
                    payload, utils::logger::Logger::LogLevel::DEBUG);

                int error_code = mosquitto_publish(
                    client_instance, nullptr, sb.str().c_str(),
                    payload.size() * sizeof(char), payload.c_str(), 1, true);

                if (error_code != MOSQ_ERR_SUCCESS)
                {
                    utils::logger::Logger::instance().Log(
                        "MiddleEndMQTT", "Failed to publish discovery message!",
                        utils::logger::Logger::LogLevel::WARNING);
                }

                id_source.second.subscribeToDeviceTopic(client_instance);

                sendDeviceStatus(id_source.second, true);
                sendDeviceState(id_source.second);
            }

            mosquitto_message_callback_set(client_instance,
                                           &mqttMessageCallBack);

            // TODO: remove this for custom server loop
            mosquitto_loop_start(client_instance);

            // Register to receive events
            core::event::EventManager::instance().registerEventHandler(this);
        }

        void MiddleEndMQTT::sendDeviceState(MQTTSourceDevice device) const
        {
            sendTriggerState(device, device.getBaseSource().isTriggered());
            sendEnabledState(device, device.getBaseSource().isEnabled());
        }

        void MiddleEndMQTT::sendDeviceStatus(MQTTSourceDevice device,
                                             bool status) const
        {
            std::string trigger_payload =
                device.getSerializedBoolStatus(status);
            std::string enable_payload = device.getSerializedBoolStatus(status);

            sendTriggerStatus(device, trigger_payload);
            sendEnabledStatus(device, enable_payload);
        }

        void MiddleEndMQTT::sendTriggerState(MQTTSourceDevice device,
                                             bool state) const
        {
            std::string state_payload =
                device.getSerializedBoolState(TRIGGER_SENSOR_ID, state);

            sendTriggerState(device, state_payload);
        }

        void MiddleEndMQTT::sendTriggerState(MQTTSourceDevice device,
                                             std::string state_payload) const
        {
            std::ostringstream topic_builder;

            topic_builder << "homeassistant/sentinelle/";
            topic_builder << device.getBaseSource().getName();
            topic_builder << "/";
            topic_builder << TRIGGER_SENSOR_ID;
            topic_builder << "/";
            topic_builder << STATE_TOPIC_NAME;

            int error_code = mosquitto_publish(
                client_instance, nullptr, topic_builder.str().c_str(),
                state_payload.size() * sizeof(char), state_payload.c_str(), 1,
                false);

            if (error_code != MOSQ_ERR_SUCCESS)
            {
                utils::logger::Logger::instance().Log(
                    "MiddleEndMQTT", "Failed to publish trigger state message!",
                    utils::logger::Logger::LogLevel::WARNING);
            }
        }

        void MiddleEndMQTT::sendTriggerStatus(MQTTSourceDevice device,
                                              std::string state_payload) const
        {
            std::ostringstream topic_builder;

            topic_builder << "homeassistant/sentinelle/";
            topic_builder << device.getBaseSource().getName();
            topic_builder << "/";
            topic_builder << TRIGGER_SENSOR_ID;
            topic_builder << "/";
            topic_builder << STATUS_TOPIC_NAME;

            int error_code = mosquitto_publish(
                client_instance, nullptr, topic_builder.str().c_str(),
                state_payload.size() * sizeof(char), state_payload.c_str(), 1,
                false);

            if (error_code != MOSQ_ERR_SUCCESS)
            {
                utils::logger::Logger::instance().Log(
                    "MiddleEndMQTT", "Failed to publish trigger state message!",
                    utils::logger::Logger::LogLevel::WARNING);
            }
        }

        void MiddleEndMQTT::sendEnabledState(MQTTSourceDevice device,
                                             bool state) const
        {
            std::string state_payload =
                device.getSerializedBoolState(ENABLED_SWITCH_ID, state);

            sendEnabledState(device, state_payload);
        }

        void MiddleEndMQTT::sendEnabledState(MQTTSourceDevice device,
                                             std::string state_payload) const
        {
            std::ostringstream topic_builder;

            topic_builder << "homeassistant/sentinelle/";
            topic_builder << device.getBaseSource().getName();
            topic_builder << "/";
            topic_builder << ENABLED_SWITCH_ID;
            topic_builder << "/";
            topic_builder << STATE_TOPIC_NAME;

            std::string temp = topic_builder.str();

            int error_code = mosquitto_publish(
                client_instance, nullptr, topic_builder.str().c_str(),
                state_payload.size() * sizeof(char), state_payload.c_str(), 1,
                false);

            if (error_code != MOSQ_ERR_SUCCESS)
            {
                utils::logger::Logger::instance().Log(
                    "MiddleEndMQTT", "Failed to publish trigger state message!",
                    utils::logger::Logger::LogLevel::WARNING);
            }
        }

        void MiddleEndMQTT::sendEnabledStatus(MQTTSourceDevice device,
                                              std::string state_payload) const
        {
            std::ostringstream topic_builder;

            topic_builder << "homeassistant/sentinelle/";
            topic_builder << device.getBaseSource().getName();
            topic_builder << "/";
            topic_builder << ENABLED_SWITCH_ID;
            topic_builder << "/";
            topic_builder << STATUS_TOPIC_NAME;

            std::string temp = topic_builder.str();

            int error_code = mosquitto_publish(
                client_instance, nullptr, topic_builder.str().c_str(),
                state_payload.size() * sizeof(char), state_payload.c_str(), 1,
                false);

            if (error_code != MOSQ_ERR_SUCCESS)
            {
                utils::logger::Logger::instance().Log(
                    "MiddleEndMQTT", "Failed to publish trigger state message!",
                    utils::logger::Logger::LogLevel::WARNING);
            }
        }

        void MiddleEndMQTT::Stop()
        {
            for (auto id_source : id_source_map)
            {
                sendDeviceStatus(id_source.second, false);
            }

            core::event::EventManager::instance().removeEventHandler(this);

            mosquitto_disconnect(client_instance);

            mosquitto_loop_stop(client_instance, false);
        }

        void MiddleEndMQTT::handleTriggerEvent(
            core::event::modules::TriggeredEvent& event)
        {
            MQTTSourceDevice dev(event.getSource());

            sendTriggerState(dev, true);
        }
        void MiddleEndMQTT::handleUnTriggerEvent(
            core::event::modules::UnTriggeredEvent& event)
        {
            MQTTSourceDevice dev(event.getSource());

            sendTriggerState(dev, false);
        }
        void MiddleEndMQTT::handleSourceStopEvent(
            core::event::modules::SourceStopEvent& event)
        {
            MQTTSourceDevice dev(event.getSource());

            sendDeviceStatus(dev, false);
        }
        void MiddleEndMQTT::handleSourceStartEvent(
            core::event::modules::SourceStartEvent& event)
        {
            MQTTSourceDevice dev(event.getSource());

            sendDeviceStatus(dev, true);
            sendDeviceState(dev);
        }

        void MiddleEndMQTT::handleEvent(core::event::Event& event)
        {
            if (!enabled)
            {
                return;
            }

            auto* trigger_event =
                dynamic_cast<core::event::modules::TriggeredEvent*>(&event);

            if (trigger_event != nullptr)
            {
                handleTriggerEvent(*trigger_event);

                return;
            }

            auto* untrigger_event =
                dynamic_cast<core::event::modules::UnTriggeredEvent*>(&event);

            if (untrigger_event != nullptr)
            {
                handleUnTriggerEvent(*untrigger_event);

                return;
            }

            auto* source_stop_event =
                dynamic_cast<core::event::modules::SourceStopEvent*>(&event);

            if (source_stop_event != nullptr)
            {
                handleSourceStopEvent(*source_stop_event);
            }

            auto* source_start_event =
                dynamic_cast<core::event::modules::SourceStartEvent*>(&event);

            if (source_start_event != nullptr)
            {
                handleSourceStartEvent(*source_start_event);
            }
        }

        void MiddleEndMQTT::HandleDeviceCommand(std::string device_name,
                                                std::string component,
                                                std::string command,
                                                std::string payload)
        {
            auto* source =
                core::orchestrator::Orchestrator::instance().GetSourceByName(
                    device_name);

            if (source == nullptr)
            {
                utils::logger::Logger::instance().Log(
                    CATEGORY_NAME,
                    "Source not found from device name received by mqtt "
                    "message!",
                    utils::logger::Logger::LogLevel::WARNING);

                return;
            }

            if (component == TRIGGER_SENSOR_ID)
            {
                return;
            }
            else if (component == ENABLED_SWITCH_ID)
            {
                if (command != SWITCH_COMMAND_NAME)
                {
                    return;
                }

                bool value = payload == SWITCH_ON_VALUE;

                source->setEnabled(value);

                sendEnabledState(MQTTSourceDevice(*source), value);

                return;
            }
        }

        std::string MiddleEndMQTT::getResponseString(int response_code)
        {
            std::ostringstream sb;

            switch (response_code)
            {
            case 0:
                sb << "Connection accepted";

                break;
            case 1:
                sb << "The Server does not support the level of the MQTT "
                      "protocol requested by the Client";

                break;
            case 2:
                sb << "The Client identifier is correct UTF-8 but not allowed "
                      "by the Server";

                break;
            case 3:
                sb << "The Network Connection has been made but the MQTT "
                      "service is unavailable";

                break;
            case 4:
                sb << "The data in the user name or password is malformed";

                break;
            case 5:
                sb << "The Client is not authorized to connect";

                break;
            default:
                sb << "Unkown response code (";
                sb << response_code;
                sb << ")";

                break;
            }

            return sb.str();
        }

        std::string MiddleEndMQTT::getReasonString(int reason_code)
        {
            std::ostringstream sb;

            switch (reason_code)
            {
            case 0:
                sb << "Disconnected successfully";

                break;
            case 4:
                sb << "Disconnect with Will Message";

                break;
            case 7:
                sb << "Connection lost";

                break;
            case 128:
                sb << "Unspecified error";

                break;
            case 129:
                sb << "Malformed Packet";

                break;
            case 130:
                sb << "Protocol Error";

                break;
            case 131:
                sb << "Implementation specific error";

                break;
            case 135:
                sb << "Not authorized";

                break;

            case 137:
                sb << "Server busy";

                break;
            case 139:
                sb << "Server shutting down";

                break;
            case 140:
                sb << "Bad authentication method";

                break;
            case 141:
                sb << "Keep Alive timeout";

                break;
            case 142:
                sb << "Session taken over";

                break;
            case 143:
                sb << "Topic Filter invalid";

                break;
            case 144:
                sb << "Topic Name invalid";

                break;
            case 147:
                sb << "Receive Maximum exceeded";

                break;
            case 148:
                sb << "Topic Alias invalid";

                break;
            case 149:
                sb << "Packet too large";

                break;
            case 150:
                sb << "Message rate too high";

                break;
            case 151:
                sb << "Quota exceeded";

                break;
            case 152:
                sb << "Administrative action";

                break;
            case 153:
                sb << "Payload format invalid";

                break;
            case 154:
                sb << "Retain not supported";

                break;
            case 155:
                sb << "QoS not supported";

                break;
            case 156:
                sb << "Use another server";

                break;
            case 157:
                sb << "Server moved";

                break;
            case 158:
                sb << "Shared Subscription not supported";

                break;
            case 159:
                sb << "Connection rate exceeded";

                break;
            case 160:
                sb << "Maximum connect time";

                break;
            case 161:
                sb << "Subscription Identifiers not supported";

                break;
            case 162:
                sb << "Wildcard Subscription not supported";

                break;
            default:
                sb << "Unkown reason code (";
                sb << reason_code;
                sb << ")";

                break;
            }

            return sb.str();
        }

        void MiddleEndMQTT::OnConnect(mosquitto*, int response)
        {
            if (response != 0)
            {
                std::ostringstream sb;

                sb << "Error while connecting to broker: ";
                sb << getResponseString(response);

                utils::logger::Logger::instance().Log(
                    CATEGORY_NAME, sb.str(),
                    utils::logger::Logger::LogLevel::WARNING);
            }
            else
            {
                utils::logger::Logger::instance().Log(
                    CATEGORY_NAME, "Connected to broker successfully",
                    utils::logger::Logger::LogLevel::INFO);
            }
        }
        void MiddleEndMQTT::OnDisconnect(mosquitto*, int reason)
        {
            if (reason == 0)
            {
                utils::logger::Logger::instance().Log(
                    CATEGORY_NAME, "Disconnected from broker successfully.",
                    utils::logger::Logger::LogLevel::DEBUG);
            }
            else
            {
                std::ostringstream sb;

                sb << "Disconnected from broker unexpectedly: ";
                sb << getReasonString(reason);
                sb << ", trying to reconnect...";

                utils::logger::Logger::instance().Log(
                    CATEGORY_NAME, sb.str(),
                    utils::logger::Logger::LogLevel::INFO);
            }
        }

        void mqttMessageCallBack(struct mosquitto*, void* data,
                                 const mosquitto_message* message)
        {
            MiddleEndMQTT* context = (MiddleEndMQTT*)data;

            std::string topic(message->topic);

            if (!topic.starts_with(MQTTSourceDevice::topic_prefix))
            {
                return;
            }

            std::vector<std::string> parts =
                MiddleEndMQTT::split(topic, TOPIC_PATH_DELIMITER);

            if (parts.size() < 3)
            {
                return;
            }

            std::string device = parts[DEVICE_NAME_PATH_POSITION];
            std::string component = parts[COMPONENT_PATH_POSITION];
            std::string command = parts[COMMAND_PATH_POSITION];

            std::size_t payload_char_length =
                message->payloadlen / sizeof(char);
            std::string payload((char*)message->payload, payload_char_length);

            context->HandleDeviceCommand(device, component, command, payload);
        }

        std::vector<std::string> MiddleEndMQTT::split(const std::string& input,
                                                      char delimiter)
        {
            std::vector<std::string> res;
            size_t last_pos = 0;

            while (input.find(delimiter, last_pos) < input.length())
            {
                size_t temp = input.find(delimiter, last_pos);
                res.push_back(input.substr(last_pos, temp - last_pos));

                last_pos = temp + 1;
            }

            if (last_pos < input.length())
            {
                res.push_back(input.substr(last_pos));
            }

            return res;
        }

        void mqttConnectCallBack(struct mosquitto* client, void* data,
                                 int response)
        {
            MiddleEndMQTT* context = (MiddleEndMQTT*)data;

            context->OnConnect(client, response);
        }

        void mqttDisconnectCallBack(struct mosquitto* client, void* data,
                                    int reason)
        {
            MiddleEndMQTT* context = (MiddleEndMQTT*)data;

            context->OnDisconnect(client, reason);
        }

        void MiddleEndMQTT::AttachToRunning()
        {
            // No threads are managed by the mqtt middle, so nothing here

            return;
        }
    } // namespace mqtt
} // namespace middle_end
