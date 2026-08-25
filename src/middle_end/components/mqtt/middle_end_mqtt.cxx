#include "middle_end_mqtt.hh"
#include "core/event/event-manager.hh"
#include "core/event/events/event.hh"
#include "core/event/events/modules/trigger/triggered-event.hh"
#include "core/event/events/modules/trigger/untriggered-event.hh"
#include "core/orchestrator/orchestrator.hh"
#include "middle_end/components/mqtt/ha_mqtt_device.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data-module.hh"
#include "utils/logger/logger.hh"

#include <json/json.h>
#include <cstddef>
#include <mosquitto.h>
#include <mosquitto/defs.h>
#include <sstream>
#include <stdexcept>
#include <string>

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
            enabled = utils::config::DataModule::readScalarOrError(node, ENABLED_NAME, middle_end_mqtt_name) == "true";
            broker_address = utils::config::DataModule::readScalarOrError(node, BROKER_ADDRESS_NAME, middle_end_mqtt_name);

            std::string broker_port_str = utils::config::DataModule::readScalarOrError(node, BROKER_PORT_NAME, middle_end_mqtt_name);
            std::string broker_keepalive_str = utils::config::DataModule::readScalarOptional(node, BROKER_KEEPALIVE_NAME).value_or(BROKER_KEEPALIVE_DEFAULT);

            username = utils::config::DataModule::readScalarOptional(node, USERNAME_NAME);
            password = utils::config::DataModule::readScalarOptional(node, PASSWORD_NAME);

            if (username.has_value() != password.has_value())
            {
                throw std::runtime_error("[MiddleEndMQTT]: If username or password is specified the other one must also be specified!");
            }

            auto source_list = utils::config::DataModule::readSequenceOrError(node, MANAGED_SOURCE_LIST_NAME, middle_end_mqtt_name);

            try
            {
                broker_port = std::stoi(broker_port_str);
                broker_keepalive = std::stoi(broker_keepalive_str);
            }
            catch (const std::invalid_argument&)
            {
                throw std::runtime_error("[MiddleEndMQTT]: broker_port and broker_keepalive should be integers!");
            }
            catch (const std::out_of_range&)
            {
                throw std::runtime_error("[MiddleEndMQTT]: Integer for broker_port and broker_keepalive should not be too big!");
            }

            for (size_t i = 0; i < source_list.size(); i++)
            {
                if (!source_list[i].IsScalar())
                {
                    throw std::runtime_error("[MiddleEndMQTT]: elements in sources of mqtt should be strings!");
                }

                auto source_name = source_list[i].Scalar();

                sources_name.push_back(source_name);
            }
        }

        void MiddleEndMQTT::Init()
        {
            //Init mosquitto lib before any other function
            mosquitto_lib_init();

            for (auto source_name : sources_name)
            {
                auto* source = core::orchestrator::Orchestrator::instance().GetSourceByName(source_name);

                if (source == nullptr)
                {

                    throw std::runtime_error("[MiddleEndMQTT]: source from mqtt config not found!");
                }

                MQTTSourceDevice dev(*source);

                id_source_map.insert({dev.getBaseSource().getName(), dev});
            }
        }

        void MiddleEndMQTT::Start()
        {
            client_instance = mosquitto_new(utils::config::ConfigManager::instance().getGeneralSettings().getServerName().c_str(),
                true,
                nullptr);

            if (client_instance == nullptr)
            {
                utils::logger::Logger::instance().Log(CATEGORY_NAME, "Unable to start the mqtt client instance!", utils::logger::Logger::LogLevel::ERROR);

                return;
            }

            if (username.has_value())
            {
                int pass_status = mosquitto_username_pw_set(client_instance, username->c_str(), password->c_str());

                if (pass_status != MOSQ_ERR_SUCCESS)
                {
                    utils::logger::Logger::instance().Log(CATEGORY_NAME, "Error while specifying the username and password!", utils::logger::Logger::LogLevel::ERROR);

                    return;
                }
            }

            int connect_status = mosquitto_connect(client_instance, broker_address.c_str(), broker_port, broker_keepalive);

            if (connect_status != MOSQ_ERR_SUCCESS)
            {
                utils::logger::Logger::instance().Log(CATEGORY_NAME, "Unable to connect to the MQTT broker!", utils::logger::Logger::LogLevel::ERROR);

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

                utils::logger::Logger::instance().Log("MiddleEndMQTT", "Advertising new MQTT device: ", utils::logger::Logger::LogLevel::DEBUG);
                utils::logger::Logger::instance().LogPlain(payload, utils::logger::Logger::LogLevel::DEBUG);

                int error_code = mosquitto_publish(client_instance, nullptr, sb.str().c_str(), payload.size()*sizeof(char), payload.c_str(), 1, true);

                if (error_code != MOSQ_ERR_SUCCESS)
                {
                    utils::logger::Logger::instance().Log("MiddleEndMQTT", "Failed to publish discovery message!", utils::logger::Logger::LogLevel::WARNING);
                }
            }

            //Register to receive events
            core::event::EventManager::instance().registerEventHandler(this);
        }

        void MiddleEndMQTT::Stop()
        {
            core::event::EventManager::instance().removeEventHandler(this);
        }

        void MiddleEndMQTT::handleTriggerEvent(core::event::modules::TriggeredEvent& event)
        {

        }
        void MiddleEndMQTT::handleUnTriggerEvent(core::event::modules::UnTriggeredEvent& event)
        {

        }

        void MiddleEndMQTT::handleEvent(core::event::Event& event)
        {
            auto* trigger_event = dynamic_cast<core::event::modules::TriggeredEvent*>(&event);

            if (trigger_event != nullptr)
            {
                handleTriggerEvent(*trigger_event);

                return;
            }

            auto* untrigger_event = dynamic_cast<core::event::modules::UnTriggeredEvent*>(&event);

            if (untrigger_event != nullptr)
            {
                handleUnTriggerEvent(*untrigger_event);

                return;
            }
        }
    }
}
