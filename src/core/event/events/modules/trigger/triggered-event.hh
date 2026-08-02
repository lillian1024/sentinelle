#pragma once

#include "core/components/sources/source.hh"
#include "core/event/events/event.hh"
#include <string>

namespace core
{
    namespace event
    {
        namespace modules
        {
            class TriggeredEvent : public Event
            {
                public:
                    TriggeredEvent(components::source::Source& source);
                    ~TriggeredEvent() = default;

                    components::source::Source& getSource();

                    EventType getType();
                private:
                    components::source::Source& source;
            };
        }
    }
}
