#include "middle_end_mqtt.hh"
#include "core/event/event-manager.hh"
#include "core/event/events/event.hh"
#include "core/event/events/modules/trigger/triggered-event.hh"
#include "core/event/events/modules/trigger/untriggered-event.hh"
#include "core/orchestrator/orchestrator.hh"
#include "utils/config/config-manager.hh"
#include "utils/config/data-module.hh"
#include "utils/logger/logger.hh"

#include <json/json.h>
#include <cstddef>
#include <functional>
#include <mosquitto.h>
#include <sstream>
#include <stdexcept>
#include <string>

#define ENABLED_NAME "enable"
#define MANAGED_SOURCE_LIST_NAME "sources"
#define PARENT_NAME "mqtt"

#define CATEGORY_NAME "MiddleEndMQTT"

namespace middle_end
{
    namespace mqtt
    {
        MiddleEndMQTT::MiddleEndMQTT(YAML::Node node)
        {
            client_instance = nullptr;
            enabled = utils::config::DataModule::readScalarOrError(node, MANAGED_SOURCE_LIST_NAME, PARENT_NAME) == "true";
            auto source_list = utils::config::DataModule::readSequenceOrError(node, MANAGED_SOURCE_LIST_NAME, PARENT_NAME);

            for (size_t i = 0; i < source_list.size(); i++)
            {
                if (!source_list[i].IsScalar())
                {
                    throw std::runtime_error("[MiddleEndMQTT]: elements in sources of mqtt should be strings!");
                }

                auto source_name = source_list[i].Scalar();

                auto* source = core::orchestrator::Orchestrator::instance().GetSourceByName(source_name);

                id_source_map.insert({hashName(source_name), source});
            }
        }

        void MiddleEndMQTT::Init()
        {
            //Init mosquitto lib before any other function
            mosquitto_lib_init();
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

            std::string topic_prefix = "homeassistant/device/";

            for (auto id_source : id_source_map)
            {
                auto device_id = hashName(id_source.second->getName());

                std::ostringstream sb;

                sb << topic_prefix;
                sb << device_id;
                sb << "/config";

                Json::Value payload_root;

                std::ostringstream payload_builder;

                payload_builder << payload_root;

                std::string payload = payload_builder.str();

                mosquitto_publish(client_instance, nullptr, sb.str().c_str(), payload.size()*sizeof(char), payload.c_str(), 2, true);
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

        std::size_t MiddleEndMQTT::hashName(std::string name)
        {
            std::hash<std::string> hasher;

            return hasher(name);
        }
    }
}
