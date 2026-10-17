#include "source-start-event.hh"

#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        namespace modules
        {
            SourceStartEvent::SourceStartEvent(components::source::Source& source)
                : Event("Source has stopped"),
                source(source) { }

            components::source::Source& SourceStartEvent::getSource()
            {
                return source;
            }

            EventType SourceStartEvent::getType()
            {
                return EventType::SOURCE_START;
            }
        }
    }
}
