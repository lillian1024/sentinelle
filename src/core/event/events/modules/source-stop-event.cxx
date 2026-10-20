#include "source-stop-event.hh"

#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        namespace modules
        {
            SourceStopEvent::SourceStopEvent(components::source::Source& source)
                : Event("Source has stopped"),
                source(source) { }

            components::source::Source& SourceStopEvent::getSource()
            {
                return source;
            }

            EventType SourceStopEvent::getType()
            {
                return EventType::SOURCE_STOP;
            }
        }
    }
}
