#pragma once

#include "core/components/sources/source.hh"
#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        namespace modules
        {
            class UnTriggeredEvent : public Event
            {
                public:
                    UnTriggeredEvent(components::source::Source& source);
                    ~UnTriggeredEvent() = default;

                    components::source::Source& getSource();

                    EventType getType();
                private:
                    components::source::Source& source;
            };
        }
    }
}
