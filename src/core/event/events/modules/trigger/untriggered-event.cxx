#include "untriggered-event.hh"

#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        namespace modules
        {
            UnTriggeredEvent::UnTriggeredEvent(components::source::Source& source)
                : Event("Source has been untriggered"),
                source(source) { }

            components::source::Source& UnTriggeredEvent::getSource()
            {
                return source;
            }

            EventType UnTriggeredEvent::getType()
            {
                return EventType::UNTRIGGERED;
            }
        }
    }
}
