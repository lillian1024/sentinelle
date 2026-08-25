#pragma once

#include "core/components/sources/source.hh"
#include "core/event/event-handler.hh"
#include "core/event/events/event.hh"
#include "core/event/events/modules/trigger/triggered-event.hh"
#include "core/event/events/modules/trigger/untriggered-event.hh"
#include "middle_end/middle_end.hh"
#include <cstddef>
#include <map>
#include <mosquitto/defs.h>
#include <string>
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
            protected:
                void handleEvent(core::event::Event& event);

                virtual void handleTriggerEvent(core::event::modules::TriggeredEvent& event);
                virtual void handleUnTriggerEvent(core::event::modules::UnTriggeredEvent& event);

                static std::size_t hashName(std::string name);

                bool enabled;

                mosquitto *client_instance;

                std::map<std::size_t, core::components::source::Source*> id_source_map;
        };
    }
}
