#include "triggered-event.hh"

#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        namespace modules
        {
            TriggeredEvent::TriggeredEvent(components::source::Source& source)
                : Event("Source has been triggered"),
                source(source) { }

            components::source::Source& TriggeredEvent::getSource()
            {
                return source;
            }

            EventType TriggeredEvent::getType()
            {
                return EventType::TRIGGERED;
            }
        }
    }
}
